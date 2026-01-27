#include "SCR_Drive.h"

/******************************************************************************
function:   send command
parameter:  Data: Data buffer
Info:
******************************************************************************/
static void SCR_SendCommand(UBYTE *Data)
{
    //printf("Data %x %x %x %x %x %x\r\n",Data[0],Data[1],Data[2],Data[3],Data[4],Data[5]);
    if(Data_Interface == UART_Interfac){
        UART_Write_nByte(Data,6);
    }else if(Data_Interface == I2C_Interfac){
        I2C_Write_Word(Data[2],(Data[3]<<8) | Data[4]);
    }
}


/******************************************************************************
function:   XOR Check Digit Count
parameter:  Data: Data buffer
Info:   Check data save Data [4]
    
******************************************************************************/
static void SET_Check_Digit(UBYTE *Data)
{
    Data[5] = ((((Data[0]^Data[1])^Data[2])^Data[3])^Data[4]);
}

/******************************************************************************
function:   Set working mode
parameter:  Mode:  
                  0: Switch mode
                  1: Voltage regulation mode
Info:
******************************************************************************/
void SCR_SetMode(UBYTE Mode)
{
    UBYTE ch[6]={0x57,0x68,0x01,0x00,0x00,0x00};
    ch[4] = Mode&0x01;
    SET_Check_Digit(ch);
    SCR_SendCommand(ch);
    DEV_Delay_ms(100);
}

/******************************************************************************
function:   Set channel enable
parameter:  Channel:  
                  1: Channel 1 enable
                  2: Channel 2 enable
Info:
******************************************************************************/
UBYTE CH_EN[6]={0x57,0x68,0x02,0x00,0x00,0x00};
void SCR_ChannelEnable(UBYTE Channel)
{
    if(Channel == 1){
        CH_EN[4] = 0x01|CH_EN[4];
        SET_Check_Digit(CH_EN);
        SCR_SendCommand(CH_EN);
    }else if(Channel == 2){
        CH_EN[4] = 0x02|CH_EN[4];
        SET_Check_Digit(CH_EN);
        SCR_SendCommand(CH_EN);
    }
    DEV_Delay_ms(100);
}

/******************************************************************************
function:   Set channel disable
parameter:  Channel:  
                  1: Channel 1 disable
                  2: Channel 2 disable
Info:
******************************************************************************/
void SCR_ChannelDisable(UBYTE Channel)
{
    if(Channel == 1){
        CH_EN[4] = 0xfe & CH_EN[4];
        SET_Check_Digit(CH_EN);
        SCR_SendCommand(CH_EN);
    }else if(Channel == 2){
        CH_EN[4] = 0xfd & CH_EN[4];
        SET_Check_Digit(CH_EN);
        SCR_SendCommand(CH_EN);
    }
    DEV_Delay_ms(100);
}

/******************************************************************************
function:   Set Voltage Regulation
parameter:  Channel:  
                  1: Channel 1
                  2: Channel 2
            Angle:
                   0~179 Conduction angle
Info:
******************************************************************************/
UBYTE Angle1[6]={0x57,0x68,0x03,0x00,0x00,0x00};
UBYTE Angle2[6]={0x57,0x68,0x04,0x00,0x00,0x00};
//Parameter 4 Set conduction angle 0-179
//If in mode 1, then the conduction angle greater than 90 degrees will be on
//If set to 180 the actual effect is 0
void SCR_VoltageRegulation(UBYTE Channel,  UBYTE Angle)
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
parameter:  Delay: 
                    0~255 Delay in milliseconds before reset 
Info:
******************************************************************************/
UBYTE Frequency[6]={0x57,0x68,0x05,0x00,0x32,0x00};
//0x32      50Hz   
//0x6c      60Hz
void SCR_GridFrequency(UBYTE Hz)
{
    if(Hz == 50 || Hz ==60){
        Frequency[4] = Hz;
        SET_Check_Digit(Frequency);
        SCR_SendCommand(Frequency);
    }
    DEV_Delay_ms(100);
}

/******************************************************************************
function:   SCR Reset
parameter:  Delay: 
                    0~255 Delay in milliseconds before reset 
Info:
    Reset all settings except baud rate and grid frequency,
******************************************************************************/
void SCR_Reset(UBYTE Delay)
{
    UBYTE ch[6]={0x57,0x68,0x06,0x00,0x00,0x00};
    ch[4] = Delay & 0xff;
    ch[3] = Delay >> 8;
    SET_Check_Digit(ch);
    SCR_SendCommand(ch);
    DEV_Delay_ms(100);
}

/******************************************************************************
function:   SCR Set the baud rate
parameter:  Delay: 
                    0~255 Delay in milliseconds before reset 
Info:
    Reset all settings except baud rate and grid frequency,
******************************************************************************/
void SCR_SetBaudrate(UDOUBLE Baudrate)
{
    UBYTE ch[6] ={0x57, 0x68, 0x07, 0x24, 0x00,0x00};
    Baudrate = Baudrate / 100;
    if(Baudrate>=12 && Baudrate<9216){
        ch[4] = Baudrate & 0xff;
        ch[3] = Baudrate >> 8;
        SET_Check_Digit(ch);
        SCR_SendCommand(ch);
        DEV_Delay_ms(100);
    }
    
}



