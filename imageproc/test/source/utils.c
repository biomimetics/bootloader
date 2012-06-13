/******************************************
* Name: utils.c                     
* Desc: Utility functions 
* Date: 2009-9-14
* Author: fgb, stanbaek
******************************************/

#include "init.h"


// Note: Since temp is an int, the maximun value for the delay is 8.2ms (time=8192)
//       Tried with long, but the assembly code might need to be modified
unsigned int temp;

void delay_us (unsigned int time) {  

    // Need to waste ~40 inst/us (for 40MIPS)
    // temp = 8 * time - 3; // #3 obtained testing btg delays
    // for 20MIPS 
   	temp = 4 * time - 3; // #3 obtained testing btg delays
	asm volatile("LOOP: dec _temp");	
	asm volatile("cp0 _temp");
	asm volatile("bra z, DONE");
	asm volatile("bra LOOP");
	asm volatile("DONE:");
}

void delay_ms (unsigned int time){
    int i;
    for(i=0; i<time; i++){
	delay_us(1000);
    }	
}


void control_LEDs(unsigned char cont) {

    static unsigned int count = 0;
    static unsigned char state = LED_NORMAL;

    switch (state) {

        case LED_NORMAL:
            state = cont;
            if (count == 100) {
                LED_GREEN = ~LED_GREEN;
                count = 0;
            } else {
                count++;
            }
            break;

        case LED_TURNLEFT:
            LED_GREEN = ON;
            LED_RED = OFF;
            if (cont == LED_RESET) {
                state = LED_NORMAL;
                LED_RED = OFF;
            } else if (cont != LED_NORMAL) {
                state = cont;
            }
            break;

        case LED_TURNRIGHT:
            LED_GREEN = OFF;
            LED_RED = ON;
            if (cont == LED_RESET) {
                state = LED_NORMAL;
                LED_RED = OFF;
            } else if (cont != LED_NORMAL) {
                state = cont;
            }
            break;

        default:
            break;

    }
}





