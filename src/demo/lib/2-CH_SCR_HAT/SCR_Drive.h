/*****************************************************************************
* | File        :   SCR_Drive.h
* | Author      :   Waveshare team (modified for Orange Pi Zero 2W)
* | Function    :   2-CH SCR HAT driver header
* | Info        :   UART interface only
*----------------
* |	This version:   V3.0
* | Date        :   2026-01-30
*
******************************************************************************/
#ifndef _SCR_DRIVE_H_
#define _SCR_DRIVE_H_

#include "DEV_Config.h"

/* Interface selection - UART only for Orange Pi Zero 2W */
#define UART_Interfac 1
#define I2C_Interfac  0

#define Data_Interface UART_Interfac

/* Function prototypes */
void SCR_SetMode(UBYTE Mode);
void SCR_ChannelEnable(UBYTE Channel);
void SCR_ChannelDisable(UBYTE Channel);
void SCR_VoltageRegulation(UBYTE Channel, UBYTE Angle);
void SCR_GridFrequency(UBYTE Hz);
void SCR_Reset(UBYTE Delay);
void SCR_SetBaudrate(UDOUBLE Baudrate);

#endif

