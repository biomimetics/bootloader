/******************************************************************************
* Name: radio.h
* Desc: Software module for AT86RF231 (SPI)
* Date: 2010-06-02
* Author: stanbaek
******************************************************************************/
#ifndef __RADIO_H
#define __RADIO_H

#include "payload.h"

void radioSetup(void);

/*****************************************************************************
* Function Name : radioReadId
* Description   : This function reads ID number of AT86RF231 chip
* Parameters    : A character array of size 4 to hold id value
* Return Value  : None
*****************************************************************************/

void radioReadTrxId(unsigned char *id);

unsigned char radioGetTrxState(void);

unsigned char radioGetChar(char *data);

unsigned char radioIsRxDataReady(void);

unsigned int radioRxData(char *buffer);

void radioTxData(unsigned char length, char *buffer,
                  unsigned char status, unsigned char type);

#endif // __RADIO_H
