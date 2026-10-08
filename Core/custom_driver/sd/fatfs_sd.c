/*
 * fatfs_sd.c
 *
 *  Created on: Oct 6, 2026
 *      Author: asus
 */



#include "fatfs_sd.h"
#include "main.h"

extern SPI_HandleTypeDef hspi1;

/* SD Commands */
#define CMD0   (0)          /* GO_IDLE_STATE */
#define CMD1   (1)          /* SEND_OP_COND (MMC/Legacy) */
#define CMD8   (8)          /* SEND_IF_COND */
#define CMD16  (16)         /* SET_BLOCKLEN */
#define CMD17  (17)         /* READ_SINGLE_BLOCK */
#define CMD24  (24)         /* WRITE_BLOCK */
#define CMD55  (55)         /* APP_CMD */
#define CMD58  (58)         /* READ_OCR */
#define ACMD41 (41 | 0x80)  /* SD_SEND_OP_COND */

/* Card type flags */
#define CT_MMC   0x01       /* MMC ver 3 */
#define CT_SD1   0x02       /* SD ver 1 (256MB standard capacity) */
#define CT_SD2   0x04       /* SD ver 2 */
#define CT_BLOCK 0x08       /* Block addressing */

static volatile DSTATUS Stat = STA_NOINIT;
static uint8_t CardType = 0;

static void SD_CS_Select(void) {
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
}

static void SD_CS_Deselect(void) {
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
}

static uint8_t SPI_TransmitReceive(uint8_t data) {
    uint8_t rx_data = 0xFF;
    __HAL_SPI_CLEAR_OVRFLAG(&hspi1);
    HAL_SPI_TransmitReceive(&hspi1, &data, &rx_data, 1, 100);
    return rx_data;
}

/* Transmits a raw 6-byte command frame over SPI without toggling CS */
static uint8_t SD_SendCmd_Raw(uint8_t cmd, DWORD arg) {
    uint8_t res, n;

    // Command packet
    SPI_TransmitReceive(cmd | 0x40);
    SPI_TransmitReceive((uint8_t)(arg >> 24));
    SPI_TransmitReceive((uint8_t)(arg >> 16));
    SPI_TransmitReceive((uint8_t)(arg >> 8));
    SPI_TransmitReceive((uint8_t)arg);

    // CRC
    n = 0x01;
    if (cmd == CMD0) n = 0x95;
    if (cmd == CMD8) n = 0x87;
    SPI_TransmitReceive(n);

    // Receive R1 response (wait up to 10 bytes)
    n = 10;
    do {
        res = SPI_TransmitReceive(0xFF);
    } while ((res & 0x80) && --n);

    return res;
}

/* Handles CS selection and ACMD (CMD55) prefixing atomically */
static uint8_t SD_SendCmd(uint8_t cmd, DWORD arg) {
    uint8_t res;

    SD_CS_Deselect();
    SPI_TransmitReceive(0xFF);
    SD_CS_Select();
    SPI_TransmitReceive(0xFF);

    // Handle ACMD (CMD55 prefix) without de-asserting CS
    if (cmd & 0x80) {
        cmd &= 0x7F;
        res = SD_SendCmd_Raw(CMD55, 0);
        if (res > 1) {
            SD_CS_Deselect();
            SPI_TransmitReceive(0xFF);
            return res;
        }
        SPI_TransmitReceive(0xFF); // Dummy byte between CMD55 and ACMD command
    }

    res = SD_SendCmd_Raw(cmd, arg);
    return res;
}

DSTATUS SD_disk_initialize(BYTE pdrv) {
    if (pdrv != 0) return STA_NOINIT;

    uint8_t n, cmd_res, ocr[4];
    uint16_t timeout;

    SD_CS_Deselect();
    for (n = 10; n; n--) SPI_TransmitReceive(0xFF); // 80 dummy clock cycles

    CardType = 0;

    if (SD_SendCmd(CMD0, 0) == 1) { // Card entered Idle State
        if (SD_SendCmd(CMD8, 0x1AA) == 1) { // SDv2
            for (n = 0; n < 4; n++) ocr[n] = SPI_TransmitReceive(0xFF);
            if (ocr[2] == 0x01 && ocr[3] == 0xAA) {
                timeout = 1000;
                while (SD_SendCmd(ACMD41, 1UL << 30) && --timeout) HAL_Delay(1);
                if (timeout && SD_SendCmd(CMD58, 0) == 0) {
                    for (n = 0; n < 4; n++) ocr[n] = SPI_TransmitReceive(0xFF);
                    CardType = (ocr[0] & 0x40) ? (CT_SD2 | CT_BLOCK) : CT_SD2;
                }
            }
        } else { // SDv1 (256MB cards) or MMC
            timeout = 1000;
            cmd_res = SD_SendCmd(ACMD41, 0);
            if (cmd_res <= 1) {
                CardType = CT_SD1;
                while (SD_SendCmd(ACMD41, 0) != 0 && --timeout) HAL_Delay(1);
            }

            // Fallback to CMD1 if ACMD41 isn't supported on legacy MMC card
            if (!timeout || cmd_res > 1) {
                CardType = CT_MMC;
                timeout = 1000;
                while (SD_SendCmd(CMD1, 0) != 0 && --timeout) HAL_Delay(1);
            }

            if (!timeout) CardType = 0;

            if (CardType != 0) {
                SD_SendCmd(CMD16, 512); // Force 512-byte block size
            }
        }
    }

    SD_CS_Deselect();
    SPI_TransmitReceive(0xFF);

    if (CardType) {
        Stat &= ~STA_NOINIT; // Clear STA_NOINIT -> Initialization Success!
    } else {
        Stat = STA_NOINIT;
    }

    return Stat;
}

DSTATUS SD_disk_status(BYTE pdrv) {
    if (pdrv != 0) return STA_NOINIT;
    return Stat;
}

DRESULT SD_disk_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count) {
    if (pdrv != 0 || !count) return RES_PARERR;
    if (Stat & STA_NOINIT) return RES_NOTRDY;

    // Convert sector index to Byte Address for SDv1 (256MB)
    if (!(CardType & CT_BLOCK)) sector *= 512;

    while (count--) {
        if (SD_SendCmd(CMD17, sector) == 0) {
            uint16_t timeout = 30000;
            while (SPI_TransmitReceive(0xFF) != 0xFE && --timeout);
            if (!timeout) {
                SD_CS_Deselect();
                SPI_TransmitReceive(0xFF);
                return RES_ERROR;
            }

            for (int i = 0; i < 512; i++) {
                *buff++ = SPI_TransmitReceive(0xFF);
            }

            SPI_TransmitReceive(0xFF); // Skip CRC
            SPI_TransmitReceive(0xFF);

            sector += (CardType & CT_BLOCK) ? 1 : 512;
        } else {
            SD_CS_Deselect();
            SPI_TransmitReceive(0xFF);
            return RES_ERROR;
        }
    }

    SD_CS_Deselect();
    SPI_TransmitReceive(0xFF);

    return RES_OK;
}

DRESULT SD_disk_write(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count) {
    if (pdrv != 0 || !count) return RES_PARERR;
    if (Stat & STA_NOINIT) return RES_NOTRDY;

    if (!(CardType & CT_BLOCK)) sector *= 512;

    while (count--) {
        if (SD_SendCmd(CMD24, sector) == 0) {
            SPI_TransmitReceive(0xFF);
            SPI_TransmitReceive(0xFE); // Data token

            for (int i = 0; i < 512; i++) {
                SPI_TransmitReceive(*buff++);
            }

            SPI_TransmitReceive(0xFF); // CRC
            SPI_TransmitReceive(0xFF);

            if ((SPI_TransmitReceive(0xFF) & 0x1F) != 0x05) {
                SD_CS_Deselect();
                SPI_TransmitReceive(0xFF);
                return RES_ERROR;
            }

            uint32_t timeout = 500000;
            while (SPI_TransmitReceive(0xFF) == 0x00 && --timeout);
            if (!timeout) {
                SD_CS_Deselect();
                SPI_TransmitReceive(0xFF);
                return RES_ERROR;
            }

            sector += (CardType & CT_BLOCK) ? 1 : 512;
        } else {
            SD_CS_Deselect();
            SPI_TransmitReceive(0xFF);
            return RES_ERROR;
        }
    }

    SD_CS_Deselect();
    SPI_TransmitReceive(0xFF);

    return RES_OK;
}

DRESULT SD_disk_ioctl(BYTE pdrv, BYTE cmd, void *buff) {
    if (pdrv != 0) return RES_PARERR;
    if (Stat & STA_NOINIT) return RES_NOTRDY;

    DRESULT res = RES_ERROR;
    switch (cmd) {
        case CTRL_SYNC:
            SD_CS_Select();
            if (SPI_TransmitReceive(0xFF) == 0xFF) res = RES_OK;
            SD_CS_Deselect();
            break;
        case GET_SECTOR_COUNT:
            *(DWORD*)buff = 500000; // ~256MB capacity
            res = RES_OK;
            break;
        case GET_SECTOR_SIZE:
            *(WORD*)buff = 512;
            res = RES_OK;
            break;
        case GET_BLOCK_SIZE:
            *(DWORD*)buff = 32;
            res = RES_OK;
            break;
        default:
            res = RES_PARERR;
            break;
    }
    return res;
}

