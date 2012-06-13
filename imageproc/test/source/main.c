/******************************************************************************
* Name: main.c
* Desc: 
* Date: 2010-07-08
* Author: stanbaek
*******************************************************************************/

#include "p33Fxxxx.h"
#include "init_default.h"
#include "utils.h"

int main(void) {

    SetupClock();
    SwitchClocks();

    while(1) {
        LED_1 = ~LED_1;
        delay_ms(150);
        LED_2 = ~LED_2;
        delay_ms(150);
        LED_3 = ~LED_3;
        delay_ms(150);
    }
    
}

