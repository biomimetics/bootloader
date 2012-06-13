/******************************************
* Name: utils.h                     
* Desc: Header for utility functions
* Date: 2009-8-17
* Author: fgb, stanbaek
******************************************/

#ifndef __UTIL_H
#define __UTIL_H

#define LED_RESET           0
#define LED_NORMAL          1
#define LED_TURNLEFT        2
#define LED_TURNRIGHT       3

#define LED_GREEN       _LATF0
#define LED_RED         _LATF1

#define LED_GREEN_ON    _LATF0 = 1
#define LED_RED_ON      _LATF1 = 1
#define LED_GREEN_OFF   _LATF0 = 0
#define LED_RED_OFF     _LATF1 = 0

#define TURN_LEFT       _LATB0 = 1; _LATB2 = 0
#define TURN_RIGHT      _LATB0 = 0; _LATB2 = 1
#define CENTER          _LATB0 = 0; _LATB2 = 0

#define DIR_LEFT        _LATB3 = 1
#define DIR_RIGHT       _LATB3 = 0


#define max(a,b)    (a>b)?a:b
#define min(a,b)    (a<b)?a:b

#define FALSE   0
#define TRUE    !FALSE
#define OFF     0
#define ON      !OFF  

void delay_us( unsigned int time );
void delay_ms( unsigned int time );
void control_LEDs(unsigned char cont);
#endif
