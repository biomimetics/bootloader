/******************************************************************************
* Name: main.c
* Desc: 
* Date: 2010-07-08
* Author: stanbaek
*******************************************************************************/

#include "p33Fxxxx.h"
#include "init_default.h"
#include "bootloader.h"
#include "utils.h"

int main(void) {

    SetupClock();
    SwitchClocks();
    SetupPorts();
    bootSetup();

    LED_1 = 1; LED_2 = 1; LED_3 = 1; LED_0 = 1;
    delay_ms(500);
    LED_1 = 0; LED_2 = 0; LED_3 = 0; LED_0 = 0;
    delay_ms(200);
    LED_1 = 1; LED_2 = 1; LED_3 = 1; LED_0 = 1;
    delay_ms(500);
    LED_1 = 0; LED_2 = 0; LED_3 = 0; LED_0 = 0;
        
    bootRun();
    bootReset();

    while(1);
    return 1;

}

