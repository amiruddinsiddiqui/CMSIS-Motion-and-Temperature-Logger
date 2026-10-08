/*
 * fatfs_sd.h
 *
 *  Created on: Oct 6, 2026
 *      Author: asus
 */




#ifndef FATFS_SD_H
#define FATFS_SD_H

#include "integer.h"
#include "diskio.h"

DSTATUS SD_disk_initialize(BYTE pdrv);
DSTATUS SD_disk_status(BYTE pdrv);
DRESULT SD_disk_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count);
DRESULT SD_disk_write(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count);
DRESULT SD_disk_ioctl(BYTE pdrv, BYTE cmd, void *buff);

#endif /* FATFS_SD_H */



