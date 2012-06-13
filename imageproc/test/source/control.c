/***************************************************************************
* Name: control.c
* Desc: Control Law
* Date: 2009-10-28
* Author: stanbaek
**************************************************************************/

#include "init.h"

#define STATE_FLYING_STRAIGHT       0
#define STATE_TURNING_LEFT	    1
#define STATE_TURNING_RIGHT	    2
#define ERROR_THRESHOLD             30

extern unsigned int PTPERvalue;
unsigned char Auto_Pilot = 0;

unsigned char Control_IsAutoPilot(void) {
    return Auto_Pilot;
}


void Control_Steering(Target* target) {

    static char stateTurning = STATE_FLYING_STRAIGHT;
    Target err, ref = {512, 384, 0, 0};

    err.x = ref.x - target->x;
    err.y = ref.y - target->y;

    switch (stateTurning) {

    	case STATE_FLYING_STRAIGHT:
            LED_RED_OFF;
            LED_GREEN_OFF;
            if (err.x > ERROR_THRESHOLD) {
                TURN_LEFT;
                stateTurning = STATE_TURNING_LEFT;
            } else if (err.x < -ERROR_THRESHOLD) {
                TURN_RIGHT;
                stateTurning = STATE_TURNING_RIGHT;
            } 
            break;

    	case STATE_TURNING_LEFT:
            LED_RED_ON;
            LED_GREEN_OFF;
            if (err.x < -ERROR_THRESHOLD) {
                TURN_RIGHT;
                stateTurning = STATE_TURNING_RIGHT;
            } else if (err.x < ERROR_THRESHOLD) {
                CENTER;
                stateTurning = STATE_FLYING_STRAIGHT;
            } 

    	    break;

    	case STATE_TURNING_RIGHT:
	        LED_RED_OFF;
            LED_GREEN_ON;
            if (err.x > ERROR_THRESHOLD) {
                TURN_LEFT;
                stateTurning = STATE_TURNING_LEFT;
            } else if (err.x > -ERROR_THRESHOLD) {
                CENTER;
                stateTurning = STATE_FLYING_STRAIGHT;
            } 
	        break;

    	default:
	        break;
    }

}

void Control_ManualOperation(void) {

    unsigned char command, argument;
    unsigned int PDC2value, PDC3value, dutyCycle;
    Target* target;
    Blob* blobs;
    int i;
    

    if (Bluetooth_DataRdy()) {
        command = Bluetooth_Getc();
        argument = Bluetooth_Read();

        switch (command) {
            case CMD_DCUPDATE:
                dutyCycle = (unsigned int)argument;
                PDC2value = (unsigned int)(dutyCycle/50.0 * PTPERvalue);	
    	        SetDCMCPWM(2, PDC2value, 0);
                Bluetooth_Write(argument);
                break;

            case CMD_DIRDCUPDATE:
                dutyCycle = (unsigned int)argument;
                PDC3value = (unsigned int)(dutyCycle/50.0 * PTPERvalue);	
    	        SetDCMCPWM(3, PDC3value, 0);
                Bluetooth_Write(argument);
                break;

            case CMD_STEERUPDATE:
                if (argument == CMD_TURNLEFT) {
                    TURN_LEFT;
                    control_LEDs(LED_TURNLEFT);
                } else if (argument == CMD_TURNRIGHT) {
                    TURN_RIGHT;
                    control_LEDs(LED_TURNRIGHT);
                } else if (argument == CMD_NOTURN) {
                    CENTER;
                    control_LEDs(LED_RESET);
                }
                break;

            case CMD_DCSWEEP:
                dutyCycle = (unsigned int)argument;
                while (dutyCycle <=100){
	            PDC2value = (unsigned int)(dutyCycle/50.0 * PTPERvalue);	
    	            SetDCMCPWM(2, PDC2value, 0);
                    TimerSleep(10000);
    	            SetDCMCPWM(2, 0, 0);
                    TimerSleep(1000);
                    dutyCycle += 2;
                }
                break;   

            case CMD_WIIMOTE:
                if (argument == CMD_WIIREAD) {
                    blobs = Wiimote_ReadBlobs();
                    for (i = 0; i < 4; i++) {
                        Bluetooth_PutWord(blobs[i].x);
                        Bluetooth_PutWord(blobs[i].y);
                        Bluetooth_PutWord(blobs[i].size);
                    }
                    target = Tracking_GetCurrentTarget();
                    Bluetooth_PutWord(target->x);
                    Bluetooth_PutWord(target->y);
                    Bluetooth_PutWord(target->distance);
                    //Bluetooth_PutString(12, Wiimote_GetRawData());
                    //Util_ToggleRed();
                } 
                break;

            default:
                break;
        }

    }

}

