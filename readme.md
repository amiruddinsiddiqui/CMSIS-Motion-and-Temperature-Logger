# STM32F401RE MPU6500 MotionSense

A real-time embedded motion monitoring and data logging system built using **STM32F401RE**, **MPU6500**, **ILI9341 TFT**, and an **SD card**.

The system reads acceleration, gyroscope, and temperature data from the MPU6500 using **I2C and DMA**, processes the data on the STM32, displays it on the TFT, logs it to an SD card, and sends telemetry to a PC for visualization over UART.

![Prototype](documentation/mpu_data_logger_prototype_img.jpeg)

## 1. Project Overview

This project is designed as an embedded data-acquisition system.

The STM32 periodically reads the MPU6500 sensor and generates:

- Acceleration: `Ax, Ay, Az`
- Gyroscope: `Gx, Gy, Gz`
- Temperature
- RTC timestamp

The processed data is simultaneously used for:

- Live display on the **ILI9341 TFT**
- CSV data logging on the **SD card**
- UART transmission to a PC
- Real-time visualization using Python

The stored data can also be used for **post-flight analysis**, such as analyzing acceleration, rotation, vibration, and orientation data after a rocket or other flight-test experiment.

## 2. System Architecture

```mermaid
flowchart TD

    %% =====================================================
    %% APPLICATION LAYER
    %% =====================================================
    subgraph APP["APPLICATION LAYER"]
        A1["app.c"]
        A2["10 Hz Sensor Trigger"]
        A3["Sensor Data Processing Flow"]
        A4["Logger_WriteData()"]
        A5["TFT_DisplayData()"]
    end

    %% =====================================================
    %% MIDDLEWARE / SERVICES
    %% =====================================================
    subgraph MID["MIDDLEWARE / SERVICE LAYER"]
        M1["motion_and_temp.c<br/>Raw Data → Ax/Ay/Az<br/>Gx/Gy/Gz + Temperature"]
        M2["FatFs<br/>File System"]
        M3["RTC<br/>Timestamp Service"]
        M4["Data Formatting<br/>CSV / Telemetry"]
    end

    %% =====================================================
    %% DRIVER LAYER
    %% =====================================================
    subgraph DRV["HARDWARE DRIVER LAYER"]
        D1["mpu6500.c"]
        D2["i2c.c"]
        D3["dma.c"]
        D4["sd_functions.c / user_diskio.c"]
        D5["spi.c"]
        D6["TFT_ILI9341.c"]
        D7["uart.c"]
        D8["rtc.c"]
        D9["tim.c"]
    end

    %% =====================================================
    %% HARDWARE LAYER
    %% =====================================================
    subgraph HW["HARDWARE LAYER"]
        H1["STM32F401RE"]
        H2["MPU6500"]
        H3["SD Card"]
        H4["ILI9341 TFT"]
        H5["USB / UART"]
    end

    %% =====================================================
    %% HOST LAYER
    %% =====================================================
    subgraph HOST["HOST / PC LAYER"]
        P1["Python / pySerial"]
        P2["Serial Monitor"]
        P3["Visualization / Analysis"]
    end

    %% =====================================================
    %% APPLICATION FLOW
    %% =====================================================

    A1 --> A2
    A2 -->|"Trigger"| D9
    D9 -->|"Interrupt"| A3

    A3 --> D1
    D1 --> D2
    D2 --> D3
    D3 -->|"DMA Complete"| A3

    %% =====================================================
    %% PROCESSING
    %% =====================================================

    A3 --> M1
    M1 --> M3
    M3 --> M4

    %% =====================================================
    %% OUTPUTS
    %% =====================================================

    M4 --> A4
    M4 --> A5

    A4 --> M2
    M2 --> D4
    D4 --> D5
    D5 --> H3

    A5 --> D6
    D6 --> D5
    D5 --> H4

    M4 --> D7
    D7 --> H5

    %% =====================================================
    %% SENSOR HARDWARE
    %% =====================================================

    D1 --> H2
    D2 --> H2

    %% =====================================================
    %% HOST
    %% =====================================================

    H5 --> P1
    P1 --> P2
    P2 --> P3

    %% =====================================================
    %% STYLES
    %% =====================================================

    style APP fill:#7c2d12,stroke:#f97316,color:#fff
    style MID fill:#065f46,stroke:#10b981,color:#fff
    style DRV fill:#1e3a8a,stroke:#3b82f6,color:#fff
    style HW fill:#374151,stroke:#9ca3af,color:#fff
    style HOST fill:#581c87,stroke:#a855f7,color:#fff
```

## 3. Implementation & Output

The STM32 uses a timer to periodically start an MPU6500 DMA transfer. After the DMA transfer is complete, the sensor data is processed and timestamped.

A simplified version of the application flow is:

```c
if (trigger_mpu_read_timer)
{
    trigger_mpu_read_timer = 0;
    MPU6500_Start_Read_DMA();
}

if (mpu_data_ready)
{
    mpu_data_ready = 0;

    process_mpu6500_motion_and_temp();

    RTC_GetTime(&year, &month, &day,
                &hour, &minute, &second);

    Logger_WriteData(year, month, day,
                     hour, minute, second,
                     Ax, Ay, Az,
                     Gx, Gy, Gz,
                     Tempr);

    TFT_DisplayData();
}
```

The MPU6500 driver reads the sensor registers starting from `0x3B` and receives the 14-byte sensor block using DMA.

Example logged CSV data:

```text
Timestamp,Ax,Ay,Az,Gx,Gy,Gz,Temp
2026-10-05 21:36:44,-0.32,0.16,0.95,-1.45,0.64,1.06,32.26
```

### Real-Time 3D Visualization

![MPU6500 3D Visualizer](documentation/mpu6500_3d_visualizer.png)

The Python/pySerial application receives the STM32 telemetry and displays the calculated orientation in real time.

### SD Card Data Logging

![SD Card Data Logging](documentation/sdcard_mpu_data_loggings.png)

The sensor readings are stored on the SD card as timestamped CSV data, allowing the recorded data to be analyzed later using Python, Excel, MATLAB, or other analysis tools.

## 4. Hardware & Applications

### Hardware

- STM32F401RE Nucleo
- MPU6500 6-axis IMU
- ILI9341 2.4" TFT
- SD card module

### Technologies

- CMSIS/Bare-Metal C
- STM32F4XX
- I2C
- DMA
- SPI
- UART
- RTC
- FatFs
- Python
- pySerial

### Possible Applications

- Embedded motion monitoring
- Flight-data acquisition
- UAV and robotics experiments
- Orientation and vibration monitoring
- Rocket flight-data recording
- **Post-rocket-flight data analysis**
- Offline sensor-data analysis

---

**made by Amiruddin**