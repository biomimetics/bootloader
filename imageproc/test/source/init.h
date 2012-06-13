/*
 * Signatures for initialization functions
 * that configure the dsPIC peripherals.
 *
 * Created: 2009-4-2
 * Author: AMH, fgb, stanbaek
 */

#include "p33Fxxxx.h"

#include "ports.h"
#include "interrupts.h"
#include "uart.h"
#include "pwm.h"
#include "adc.h"
#include "timer.h"
#include "i2c.h"
#include "dfmem.h"
#include "spi.h"

#include "math.h"

#include "ovcam.h"
#include "utils.h"
#include "wiimote.h"
#include "bluetooth.h"
#include "tracking.h"
#include "control.h"
//#include "consts.h"

#define MIPS	40

typedef char *String;

#ifndef __INIT_H
#define __INIT_H
#define SLEEP 0



void SetupPorts(void);
void SetupInterrupts(void);
void SetupUART2(void);
void SetupI2C(unsigned int sclk);
void SetupSPI(void);
void SetupPWM(void);
void SetupADC(void);
void SetupCamera(void);
void SetupTimer1(void);
void SetupTimer2(void);
void SetupTimer3(void);
void SetupTimer4(void);
// void SetupPID(pidT *pid, double pGain, double iGain, double dGain, double iMin, double iMax);

#endif
