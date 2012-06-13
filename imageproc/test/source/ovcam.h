/*********************************
* Name: ovcam.h
* Desc: Header for OV7660 Camera
* Date: 2007-9-10
* Author: fgb
*********************************/

#include "p33Fxxxx.h"

// SCCB Function Definitions
void SCCB_SetupOV7660(void);
void SCCB_Write( unsigned char subaddr, unsigned char data );
unsigned char SCCB_Read( unsigned char subaddr );
void SCCB_SendByte( unsigned char byte );
unsigned char SCCB_ReceiveByte(void);
void SCCB_NotAcknowledge (void);
void SCCB_StartTransmission (void);
void SCCB_EndTransmission (void);
extern void getRow(void *row, void *startPostions, void *lengths);

// Device Slave Addresses
#define idaddr_w    0x42
#define idaddr_r    0x43

// Camera Pins
#define VSYNC   _RD8
#define HREF    _RD9
#define PCLK    _RD10
#define PIXEL   PORTD

/* OV7660 Control Registers */

// Common Control
#define COM1    0x04
#define COM2    0x09
#define COM3    0x0C
#define COM5    0x0E
#define COM6    0x0F
#define COM7    0x12
#define COM8    0x13
#define COM9    0x14
#define COM10   0x15
#define COM11   0x3B
#define COM12   0x3C
#define COM13   0x3D
#define COM15   0x40
#define COM16   0x41
#define COM17   0x42

// Data Format
#define CLKRC   0x11
#define PSHFT   0x1B
#define TSLB    0x3A
#define MVFP    0x1E

#define HREFreg 0x32
#define HSTART  0x17
#define HSTOP   0x18

#define VREF    0x03
#define VSTRT   0x19
#define VSTOP   0x1A

// Edge Enhancement
#define EDGE    0x3F
#define DSPC2   0xA0

// Banding Filter
#define BD50ST  0x9D
#define BD60ST  0x9E
#define HV      0x69

#define DBLV    0x6B

// Gain/Exposure
#define GAIN    0x00
#define BLUE    0x01
#define RED     0x02

#define AECH    0x10

#define AEW     0x24
#define AEB     0x25

// Dummy Line
#define DM_LNL  0x92
#define DM_LNH  0x93

// Array
#define CHLF    0x33
#define ARBLM   0x34

// Matrix Coefficients
#define  MTX1   0x4F
#define  MTX2   0x50
#define  MTX3   0x51
#define  MTX4   0x52
#define  MTX5   0x53
#define  MTX6   0x54
#define  MTX7   0x55
#define  MTX8   0x56
#define  MTX9   0x57
#define  MTXS   0x58

// ADC
#define ACOM    0x38
#define OFON    0x39
