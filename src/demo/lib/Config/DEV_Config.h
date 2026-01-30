/*****************************************************************************
* | File        :   DEV_Config.h
* | Author      :   Waveshare team (modified for Orange Pi Zero 2W)
* | Function    :   Hardware underlying interface - Orange Pi / Ubuntu
* | Info        :   Simplified for Orange Pi Zero 2W with UART5
*----------------
* |	This version:   V3.0
* | Date        :   2026-01-30
* | Info        :   Orange Pi Zero 2W / Ubuntu 22.04 only
*
******************************************************************************/
#ifndef _DEV_CONFIG_H_
#define _DEV_CONFIG_H_

#include <stdint.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "sysfs_gpio.h"
#include "dev_hardware_UART.h"
#include "Debug.h"

/**
 * Data types
**/
#define UBYTE   uint8_t
#define UWORD   uint16_t
#define UDOUBLE uint32_t

/**
 * Orange Pi Zero 2W UART5 device
 * Pin 8: UART5_TX (PC0)
 * Pin 10: UART5_RX (PC1)
**/
#define UART5_DEVICE "/dev/ttyS5"

/**
 * Orange Pi Zero 2W GPIO (H618 SoC)
 * GPIO number formula: (Port - 'A') * 32 + Pin
 * Example: PC11 = (2 * 32) + 11 = 75
**/
#define GPIO_PORT_A  0
#define GPIO_PORT_C  64
#define GPIO_PORT_G  192
#define GPIO_PORT_H  224
#define GPIO_PORT_I  256

/*---------- Module functions ----------*/
uint8_t DEV_ModuleInit(void);
void    DEV_ModuleExit(void);

/*---------- GPIO functions ----------*/
void DEV_GPIO_Mode(UWORD Pin, UWORD Mode);
void DEV_Digital_Write(UWORD Pin, uint8_t Value);
uint8_t DEV_Digital_Read(UWORD Pin);

/*---------- Delay function ----------*/
void DEV_Delay_ms(UDOUBLE xms);

/*---------- UART functions ----------*/
void DEV_UART_Init(const char *Device);
void UART_Write_Byte(uint8_t data);
int  UART_Read_Byte(void);
void UART_Set_Baudrate(uint32_t Baudrate);
int  UART_Write_nByte(uint8_t *pData, uint32_t Len);

#endif
