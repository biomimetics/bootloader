/******************************************************************************
* Name: main.c
* Desc: 
* Date: 2010-07-08
* Author: stanbaek
*******************************************************************************/

#include "p33Fxxxx.h"
#include "init_default.h"
#include "radio_polling.h"
#include "bootloader.h"
#include "utils.h"

int main(void) {

    SetupClock();
    SwitchClocks();
    SetupPorts();
    //lcdSetup(4,20); // 4 rows & 20 cols
    radioSetup();   // polling mode
    bootSetup();

    LED_1 = 1; LED_2 = 1; LED_3 = 1;
    delay_ms(500);
    LED_1 = 0; LED_2 = 0; LED_3 = 0;
    delay_ms(200);
    LED_1 = 1; LED_2 = 1; LED_3 = 1;
    delay_ms(500);

    // check if radio is working and at the right state.
    while (radioGetTrxState() != 0x16);
        
    LED_1 = 0;
    LED_2 = 0;
    LED_3 = 0;
    
    //print("Ready");
    //bootTestRadio();
    bootRun();
    bootReset();

    while(1);
    return 1;

}

