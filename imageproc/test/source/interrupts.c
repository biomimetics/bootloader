/*************************************************
 * Interrupt Handlers
 *
 * Created: 08/28/2009
 * Author: stanbaek, amh, fgb
 *************************************************/

#include "init.h"

struct time {
    unsigned int hour;
    unsigned int sec;
    unsigned int msec;
    float tic;  // 0.001 seconds, keeps increasing
} Timer;


void TimerReset(void) {
    Timer.hour = 0;
    Timer.sec = 0;
    Timer.msec = 0;
    Timer.tic = 0;
}


void TimerSleep(float dtic) {
    tic();
    while (toc() < dtic);
}

void tic(void) {
    Timer.tic = 0.0;
}

float toc(void) {
    return Timer.tic;
}


/**************************************************
 * Interrupt hander for battery supervisor
 * ************************************************/
void __attribute__((interrupt, no_auto_psv)) _INT0Interrupt(void) {

    _INT0IF = 0;    // Clear the interrupt flag
    // unsigned int i, j;
    _LATF0 = 0;
	//Stop the motor
//    SetDCMCPWM(2, 0, 0);
//    for (j=0; j<5; j++) {
//        asm volatile("btg   PORTF, #1");
//        for (i=0; i<100; i++) { Delay_us(5000); }   // Waste approximatelly .5s
//    };
}

/***************************************************************
 *    
 *
 ****************************************************************/
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void) {
    
    _T1IF = 0;

}




/******************************************************************
 *  Bluetooth Communicator
 *  Control Law                                          
 * ****************************************************************/
void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void) {

//    Blob* blobs;
//    Target* target;

//    Wiimote_CaptureData();
//    Wiimote_ConvertBlobs(0);
//    blobs = Wiimote_ReadBlobs();
//    target = Tracking_FindTarget(blobs);
/*
    if (Control_IsAutoPilot()) {
        Control_Steering(target);
    }
*/
//    Control_ManualOperation(); 
    control_LEDs(LED_NORMAL);

    _T2IF = 0;

}



void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void) {

    _T3IF = 0;

}


/****************************************************************
 * Real Time Clock
 *
 * *************************************************************/
void __attribute__((interrupt, no_auto_psv)) _T4Interrupt(void) {

    Timer.msec++;
    if (Timer.msec == 1000) {
        Timer.msec = 0;
        Timer.sec++;
        if (Timer.sec == 3600) {
            Timer.sec = 0;
            Timer.hour++;
        } 
    }

    _T4IF = 0;

}


