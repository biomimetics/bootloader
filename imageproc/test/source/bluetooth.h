/*********************************
* Name: bluetooth.h
* Desc: Header for Bluetooth Communication
* Date: 2009-8-10
* Author: stanbaek
*********************************/

#include "p33Fxxxx.h"

#define DataWait            100	// uart_data_wait timeout value 

#define CMD_DCUPDATE        101
#define CMD_DCSWEEP         106
#define CMD_DIRDCUPDATE     110

#define CMD_STEERUPDATE     102
#define CMD_TURNLEFT        103
#define CMD_TURNRIGHT       104
#define CMD_NOTURN          105

#define CMD_DIRECTION       107
#define CMD_DIRFORWARD      108
#define CMD_DIRBACKWARD     109

#define CMD_WIIMOTE         121
#define CMD_WIIREAD         122
#define CMD_WIICONTREAD     123


unsigned char Bluetooth_Handshake(void);
unsigned int Bluetooth_Flushout(void);
char Bluetooth_Busy(void);
char Bluetooth_DataRdy(void);
unsigned int Bluetooth_Getc(void);
void Bluetooth_Putc(unsigned int data);
void Bluetooth_PutWord(unsigned int data);
void Bluetooth_Write(unsigned int data);
unsigned int Bluetooth_Read(void);
unsigned int Bluetooth_GetString(unsigned int length, unsigned int *buffer);
void Bluetooth_PutString(unsigned int length, unsigned int *buffer);


