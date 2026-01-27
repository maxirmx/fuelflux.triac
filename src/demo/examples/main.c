#include <stdio.h>      //printf()
#include <stdlib.h>     //exit()
#include <signal.h>
#include <stdio.h>

#include "SCR_Drive.h"

void  Handler(int signo)
{
    //System Exit
    printf("\r\nHandler:Program stop\r\n"); 
    SCR_VoltageRegulation(1,0);
    SCR_VoltageRegulation(2,0);
    SCR_ChannelDisable(1);
    SCR_ChannelDisable(2);
    DEV_ModuleExit();
    exit(0);
}


int main(int argc, char **argv)
{
    // Exception handling:ctrl + c
    signal(SIGINT, Handler);
    
    if (DEV_ModuleInit()==1)return 1;
    
    if(Data_Interface == UART_Interfac){
        DEV_UART_Init("/dev/ttyS0");
        printf("UART\r\n");
    }else if(Data_Interface == I2C_Interfac){
        DEV_I2C_Init(I2C_ADDRESS);
        printf("I2C\r\n");
    }
    
    int angle = 0;

    SCR_SetMode(1);
    SCR_VoltageRegulation(1,0);
    SCR_VoltageRegulation(2,0);
    SCR_ChannelEnable(1);
    SCR_ChannelEnable(2);
    
    while(1){
        DEV_Delay_ms(100);
        if(angle <180){
            SCR_VoltageRegulation(1,angle%180);
            SCR_VoltageRegulation(2,angle%180);
        }else {
            SCR_VoltageRegulation(1,180 - angle%180);
            SCR_VoltageRegulation(2,180 - angle%180);
            
        }
        angle = angle+1;
        if(angle>=360){
            angle = 0;
        }

    }
    DEV_ModuleExit();
    return 0; 
}
