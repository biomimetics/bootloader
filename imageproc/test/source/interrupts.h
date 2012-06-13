/*
 * A file that contains all signatures for interrupt handlers
 *
 * Created: 04/02/2009
 * Author: AMH
 */

#include "p33Fxxxx.h"

#ifndef __INTERRUPTS_H
#define __INTERRUPTS_H

#define ADC_BUFFER_SIZE 6000 //Should be good for 3 seconds of data at 1KHz

void TimerReset(void);
void TimerSleep(float dtic);
void tic(void);
float toc(void);
void __attribute__((interrupt, no_auto_psv)) _INT0Interrupt(void);
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void);
void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void);
void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void);
void __attribute__((interrupt, no_auto_psv)) _T4Interrupt(void);
/*
extern volatile unsigned int t1_Duration;
extern volatile long adcAvg;
extern volatile unsigned int t1_ticks;
extern volatile unsigned int t1_ms;
extern volatile unsigned int t1_secs;
extern volatile unsigned char runOsc;
extern pidT myPID;
extern oscT myOsc;
extern volatile float reference;
extern volatile unsigned char saveToBuffer;
extern volatile unsigned char adcBuffer[ADC_BUFFER_SIZE];
extern volatile unsigned int bufferPos;
*/

#endif
