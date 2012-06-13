/*********************************************************************************************************
* Name: main.c
* Desc: Image Processing of OV7660
* Date: 2007-8-8
* Author: fgb
*********************************************************************************************************/

#include "init.h"

// extern unsigned char convStep(unsigned int prev, unsigned int current, unsigned int next);

int main ( void ) {

    /* Initialization */
    SetupPorts();
    SetupInterrupts();
    // SetupUART2();
    // SetupI2C(400);
    // SetupSPI();
    // SetupPWM();
    // SetupTimer1();
    SetupTimer2();
    // SetupTimer3(); 
    // SetupTimer4();

    /// OV7660 CameraChip through SCCB
    // _RC13 = 0; // PDWN: CAM On
    // _RC14 = 1; // REG2 On
    // delay_us(5000);
    // SCCB_SetupOV7660();

/*********************************************************
 *                 Main Program
 ********************************************************/

    LED_RED = ON;
    LED_GREEN = ON;
    // Bluetooth_Handshake();
    LED_GREEN = OFF;
    // Wiimote_AdvancedSetup(1, 0x03);
    LED_RED = OFF;
    // TimerReset(); 

    // EnableIntT1;
    EnableIntT2;
    // EnableIntT3;
    // EnableIntT4;


    // LED_GREEN = ON;
    while (1);

    return 0;
}

