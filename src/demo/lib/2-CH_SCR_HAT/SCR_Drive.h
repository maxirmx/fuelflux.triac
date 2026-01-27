#ifndef _SCR_DRIVE_H_
#define _SCR_DRIVE_H_

#include "DEV_Config.h"

#define UART_Interfac 1
#define I2C_Interfac 0



#define Data_Interface UART_Interfac

#define I2C_ADDRESS  0x47

void SCR_SetMode(UBYTE Mode);
void SCR_ChannelEnable(UBYTE Channel);
void SCR_ChannelDisable(UBYTE Channel);
void SCR_VoltageRegulation(UBYTE Channel,  UBYTE Angle);
void SCR_GridFrequency(UBYTE Hz);
void SCR_Reset(UBYTE Delay);
void SCR_SetBaudrate(UDOUBLE Baudrate);
#endif

