/*****************************************************************************
* | File        :   SCR_Drive.c
* | Author      :   Waveshare team (modified for Orange Pi Zero 2W)
* | Function    :   2-CH SCR HAT driver - UART interface
* | Info        :   Simplified for Orange Pi Zero 2W
*----------------
* |	This version:   V3.0
* | Date        :   2026-01-30
*
******************************************************************************/
#include "SCR_Drive.h"

/******************************************************************************
function:   Send command via UART
parameter:  
    Data: 6-byte command buffer
******************************************************************************/
static void SCR_SendCommand(UBYTE *Data)
{
    UART_Write_nByte(Data, 6);
}

/******************************************************************************
function:   Calculate XOR checksum
parameter:  
    Data: Command buffer (checksum stored in Data[5])
******************************************************************************/
static void SET_Check_Digit(UBYTE *Data)
{
    Data[5] = ((((Data[0]^Data[1])^Data[2])^Data[3])^Data[4]);
}

/******************************************************************************
function:   Set working mode
parameter:  
    Mode: 0 = Switch mode, 1 = Voltage regulation mode
******************************************************************************/
void SCR_SetMode(UBYTE Mode)
{
    UBYTE ch[6] = {0x57, 0x68, 0x01, 0x00, 0x00, 0x00};
    ch[4] = Mode & 0x01;
    SET_Check_Digit(ch);
    SCR_SendCommand(ch);
    DEV_Delay_ms(100);
}

/******************************************************************************
function:   Enable channel
parameter:  
    Channel: 1 or 2
******************************************************************************/
static UBYTE CH_EN[6] = {0x57, 0x68, 0x02, 0x00, 0x00, 0x00};

void SCR_ChannelEnable(UBYTE Channel)
{
    if(Channel == 1){
        CH_EN[4] = 0x01 | CH_EN[4];
    }else if(Channel == 2){
        CH_EN[4] = 0x02 | CH_EN[4];
    }
    SET_Check_Digit(CH_EN);
    SCR_SendCommand(CH_EN);
    DEV_Delay_ms(100);
}

/******************************************************************************
function:   Disable channel
parameter:  
    Channel: 1 or 2
******************************************************************************/
void SCR_ChannelDisable(UBYTE Channel)
{
    if(Channel == 1){
        CH_EN[4] = 0xFE & CH_EN[4];
    }else if(Channel == 2){
        CH_EN[4] = 0xFD & CH_EN[4];
    }
    SET_Check_Digit(CH_EN);
    SCR_SendCommand(CH_EN);
    DEV_Delay_ms(100);
}

/******************************************************************************
function:   Set voltage regulation (conduction angle)
parameter:  
    Channel: 1 or 2
    Angle:   0-179 degrees conduction angle
Info:
    If in mode 1, angle > 90 degrees will be ON
    Setting 180 has same effect as 0
******************************************************************************/
static UBYTE Angle1[6] = {0x57, 0x68, 0x03, 0x00, 0x00, 0x00};
static UBYTE Angle2[6] = {0x57, 0x68, 0x04, 0x00, 0x00, 0x00};

void SCR_VoltageRegulation(UBYTE Channel, UBYTE Angle)
{
    if(Channel == 1){
        Angle1[4] = Angle;
        SET_Check_Digit(Angle1);
        SCR_SendCommand(Angle1);
    }else if(Channel == 2){
        Angle2[4] = Angle;
        SET_Check_Digit(Angle2);
        SCR_SendCommand(Angle2);
    }
    DEV_Delay_ms(100);
}

/******************************************************************************
function:   Set grid frequency
parameter:  
    Hz: 50 or 60 Hz
******************************************************************************/
static UBYTE Frequency[6] = {0x57, 0x68, 0x05, 0x00, 0x32, 0x00};

void SCR_GridFrequency(UBYTE Hz)
{
    if(Hz == 50 || Hz == 60){
        Frequency[4] = Hz;
        SET_Check_Digit(Frequency);
        SCR_SendCommand(Frequency);
    }
    DEV_Delay_ms(100);
}

/******************************************************************************
function:   SCR Reset
parameter:  
    Delay: 0-255 ms delay before reset
Info:
    Resets all settings except baud rate and grid frequency
******************************************************************************/
void SCR_Reset(UBYTE Delay)
{
    UBYTE ch[6] = {0x57, 0x68, 0x06, 0x00, 0x00, 0x00};
    ch[4] = Delay & 0xFF;
    ch[3] = Delay >> 8;
    SET_Check_Digit(ch);
    SCR_SendCommand(ch);
    DEV_Delay_ms(100);
}

/******************************************************************************
function:   Set UART baud rate
parameter:  
    Baudrate: New baud rate
******************************************************************************/
void SCR_SetBaudrate(UDOUBLE Baudrate)
{
    UBYTE ch[6] = {0x57, 0x68, 0x07, 0x00, 0x00, 0x00};
    ch[3] = (Baudrate >> 16) & 0xFF;
    ch[4] = (Baudrate >> 8) & 0xFF;
    SET_Check_Digit(ch);
    SCR_SendCommand(ch);
    DEV_Delay_ms(100);
    
    /* Update local UART baud rate */
    UART_Set_Baudrate(Baudrate);
}



