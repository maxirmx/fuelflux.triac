/*****************************************************************************
* | File        :   DEV_Config.c
* | Author      :   Waveshare team (modified for Orange Pi Zero 2W)
* | Function    :   Hardware underlying interface - Orange Pi / Ubuntu
* | Info        :   Simplified for Orange Pi Zero 2W with UART5
*----------------
* |	This version:   V3.0
* | Date        :   2026-01-30
* | Info        :   Orange Pi Zero 2W / Ubuntu 22.04 only
*
******************************************************************************/
#include "DEV_Config.h"
#include <unistd.h>
#include <fcntl.h>

/******************************************************************************
function:   GPIO Mode configuration
parameter:
    Pin:  GPIO pin number (H618 formula: (Port-'A')*32 + Pin)
    Mode: 0 = Input, 1 = Output
******************************************************************************/
void DEV_GPIO_Mode(uint16_t Pin, uint16_t Mode)
{
    SYSFS_GPIO_Export(Pin);
    if(Mode == 0 || Mode == SYSFS_GPIO_IN){
        SYSFS_GPIO_Direction(Pin, SYSFS_GPIO_IN);
    }else{
        SYSFS_GPIO_Direction(Pin, SYSFS_GPIO_OUT);
    }
}

/******************************************************************************
function:   GPIO Digital Write
parameter:
    Pin:   GPIO pin number
    Value: 0 or 1
******************************************************************************/
void DEV_Digital_Write(uint16_t Pin, uint8_t Value)
{
    SYSFS_GPIO_Write(Pin, Value);
}

/******************************************************************************
function:   GPIO Digital Read
parameter:
    Pin: GPIO pin number
return:     Pin value (0 or 1)
******************************************************************************/
uint8_t DEV_Digital_Read(uint16_t Pin)
{
    return SYSFS_GPIO_Read(Pin);
}

/******************************************************************************
function:   Delay in milliseconds
parameter:
    xms: Number of milliseconds to delay
******************************************************************************/
void DEV_Delay_ms(UDOUBLE xms)
{
    usleep(xms * 1000);
}

/******************************************************************************
function:   UART Initialization
parameter:
    Device: UART device path (e.g., "/dev/ttyS5")
******************************************************************************/
void DEV_UART_Init(const char *Device)
{
    printf("Initializing UART: %s\r\n", Device);
    DEV_HARDWARE_UART_begin((char*)Device);
    UART_Set_Baudrate(115200);
}

/******************************************************************************
function:   UART Write single byte
parameter:
    data: Byte to write
******************************************************************************/
void UART_Write_Byte(uint8_t data)
{
    DEV_HARDWARE_UART_writeByte(data);
}

/******************************************************************************
function:   UART Read single byte
return:     Byte read from UART
******************************************************************************/
int UART_Read_Byte(void)
{
    return DEV_HARDWARE_UART_readByte();
}

/******************************************************************************
function:   UART Set Baudrate
parameter:
    Baudrate: Baud rate (e.g., 9600, 115200)
******************************************************************************/
void UART_Set_Baudrate(uint32_t Baudrate)
{
    DEV_HARDWARE_UART_setBaudrate(Baudrate);
}

/******************************************************************************
function:   UART Write multiple bytes
parameter:
    pData: Pointer to data buffer
    Len:   Number of bytes to write
return:     0 on success
******************************************************************************/
int UART_Write_nByte(uint8_t *pData, uint32_t Len)
{
    DEV_HARDWARE_UART_write((const char*)pData, Len);
    return 0;
}

/******************************************************************************
function:   Module Initialize
return:     0 on success, 1 on failure
******************************************************************************/
uint8_t DEV_ModuleInit(void)
{
    printf("Orange Pi Zero 2W / Ubuntu - Initializing...\r\n");
    return 0;
}

/******************************************************************************
function:   Module Exit - cleanup resources
******************************************************************************/
void DEV_ModuleExit(void)
{
    printf("Cleaning up...\r\n");
    DEV_HARDWARE_UART_end();
}

