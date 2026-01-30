/*****************************************************************************
* | File        :   main.c
* | Function    :   2-CH SCR HAT demo for Orange Pi Zero 2W
* | Info        :   UART5 on /dev/ttyS5
*----------------
* |	This version:   V3.0
* | Date        :   2026-01-30
* | Platform    :   Orange Pi Zero 2W / Ubuntu 22.04
*
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "SCR_Drive.h"

/******************************************************************************
function:   Signal handler for graceful shutdown (Ctrl+C)
******************************************************************************/
void Handler(int signo)
{
    printf("\r\nShutting down...\r\n");
    
    /* Turn off both channels */
    SCR_VoltageRegulation(1, 0);
    SCR_VoltageRegulation(2, 0);
    SCR_ChannelDisable(1);
    SCR_ChannelDisable(2);
    
    DEV_ModuleExit();
    exit(0);
}

/******************************************************************************
function:   Main entry point
******************************************************************************/
int main(int argc, char **argv)
{
    /* Setup Ctrl+C handler */
    signal(SIGINT, Handler);
    
    /* Initialize module */
    if (DEV_ModuleInit() != 0) {
        printf("Module init failed!\r\n");
        return 1;
    }
    
    /* Initialize UART5 */
    DEV_UART_Init(UART5_DEVICE);
    
    printf("2-CH SCR HAT Demo\r\n");
    printf("Platform: Orange Pi Zero 2W\r\n");
    printf("UART: %s\r\n", UART5_DEVICE);
    printf("Press Ctrl+C to stop\r\n\r\n");
    
    int angle = 0;

    /* Set voltage regulation mode */
    SCR_SetMode(1);
    
    /* Initialize both channels to 0 */
    SCR_VoltageRegulation(1, 0);
    SCR_VoltageRegulation(2, 0);
    
    /* Enable both channels */
    SCR_ChannelEnable(1);
    SCR_ChannelEnable(2);
    
    /* Main loop - ramp angle up and down */
    while(1) {
        DEV_Delay_ms(100);
        
        if(angle < 180) {
            SCR_VoltageRegulation(1, angle % 180);
            SCR_VoltageRegulation(2, angle % 180);
        } else {
            SCR_VoltageRegulation(1, 180 - (angle % 180));
            SCR_VoltageRegulation(2, 180 - (angle % 180));
        }
        
        angle++;
        if(angle >= 360) {
            angle = 0;
        }
    }
    
    DEV_ModuleExit();
    return 0;
}
