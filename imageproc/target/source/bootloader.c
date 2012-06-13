/*
 * Copyright (c) 2010, Regents of the University of California
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * - Neither the name of the University of California, Berkeley nor the names
 *   of its contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * Bootloader module for dsPIC33F with AT86RF231 wireless transceiver.
 *
 * by Stanley S. Baek
 *
 * v.2.0.0
 *
 * Revisions:
 *  v.2.0.0: stanbaek      2010-11-08    Initial release
 *  v.2.0.1: stanbaek      2010-11-15    Removed traps.c
 *  v.2.1.0: stanbaek      2010-11-21    compiled with progmem.c
 *                      
 * Notes:
 *
 *************** IMPORTANT SOFTWARE LICENSE NOTE ******************************
 * This module is based on Bootloader for dsPIC30F/33F and PIC24F/24H Devices
 * from Microchip (http://www.microchip.com/stellent/idcplg?IdcService=
 * SS_GET_PAGE&nodeId=1824&appnote=en530200)
 *
 * Here is the original software licese agreement
 *
 ********************************************************************
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company? for its PICmicro?Microcontroller is intended and
 * supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller products. The
 * software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN “AS IS?CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *********************************************************************
 *
 */

#include "p33Fxxxx.h"
#include "bootloader.h"
#include "progmem.h"
#include "ports.h"
#include "utils.h"
#include "radio_polling.h"


#define CMD_BOOTLOADER          0xFE

#define COMMAND_NACK            0x00
#define COMMAND_ACK             0x01
#define COMMAND_READ_PM         0x02
#define COMMAND_WRITE_PM        0x03
#define COMMAND_WRITE_CM        0x07
#define COMMAND_RESET           0x08
#define COMMAND_READ_ID         0x09
#define COMMAND_GET_VERSION     0x0A
#define COMMAND_READ_GOTO       0x10

#define DELAY_ADDRESS           0x2000

#define FCY                     40000000

#define VERSION                 "2.1.0"
#define VERSION_LENGTH          5

/*-----------------------------------------------------------------------------
 *          Type Defines
-----------------------------------------------------------------------------*/

typedef union tuReg32 {
    unsigned long Val32;
    struct {
	unsigned short LW;
	unsigned short HW;
    } Word;
    char Val[4];
} uReg32;

/*-----------------------------------------------------------------------------
 *          Declaration of functions
-----------------------------------------------------------------------------*/
static void bootGetChar(char *data);
static void bootWriteBuffer(char *data, int size);


/*-----------------------------------------------------------------------------
 *          Variables
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 *          Public functions
-----------------------------------------------------------------------------*/

void bootSetup(void) {

    // Configure Timer2
    T2CONbits.T32 = 1; // to increment every instruction cycle
    IFS0bits.T3IF = 0; // Clear the Timer3 Interrupt Flag 
    IEC0bits.T3IE = 0; // Disable Timer3 Interrup Service Routine 

}


void bootTestRadio(void) {
    /*
    char buffer[128];
    unsigned int data_length;

    while(!radioIsRxDataReady());
    data_length = radioRxData(buffer);
    print(buffer);
    while(!radioIsRxDataReady());
    data_length = radioRxData(buffer);
    print(buffer);
    while(1) {
        while(!radioGetChar(&(buffer[0])));
        print(buffer);
        delay_ms(500);
    }

    radioTxData(11, "Hello world", 0, 1);
    radioTxData(13, "What the hell", 0, 1);
    radioTxData(12, "Who are you?", 0, 1);
    radioTxData(11, "Hello world", 0, 1);
    radioTxData(13, "What the hell", 0, 1);
    radioTxData(12, "Who are you?", 0, 1);
    radioTxData(11, "Hello world", 0, 1);
    radioTxData(13, "What the hell", 0, 1);
    radioTxData(12, "Who are you?", 0, 1);
    radioTxData(11, "Hello world", 0, 1);
    radioTxData(13, "What the hell", 0, 1);
    radioTxData(12, "Who are you?", 0, 1);
    radioTxData(11, "Hello world", 0, 1);
    radioTxData(13, "What the hell", 0, 1);
    radioTxData(12, "Who are you?", 0, 1);
    */
}



/******************************************************************************/
void bootRun(void) {
    
    char cmd;
    char buffer[PM_ROW_SIZE*3 + 1];
    // unsigned char lcdstr[80];

    uReg32 src_addr, delay, Temp;

    int i;


    src_addr.Val32 = DELAY_ADDRESS;
    
    delay.Val32 = pmReadMem(src_addr.Val32);

    if(delay.Val[0] == 0) {
        bootReset();
    }

    if((delay.Val32 & 0x000000FF) != 0xFF){
	// Convert seconds into timer count value 
        delay.Val32 = ((unsigned long)(FCY)) * ((unsigned long)(delay.Val[0]));

        PR3 = delay.Word.HW;
        PR2 = delay.Word.LW;

	// Enable Timer
        T2CONbits.TON=1;
    }
    
    while(1) {

        bootGetChar(&cmd);

        switch(cmd){
            case COMMAND_READ_PM:			

		bootGetChar(&(src_addr.Val[0]));
		bootGetChar(&(src_addr.Val[1]));
		bootGetChar(&(src_addr.Val[2]));
		src_addr.Val[3] = 0;

		pmReadPage(buffer, src_addr.Val32);
		bootWriteBuffer(buffer, PM_ROW_SIZE*3);

		break;

            case COMMAND_WRITE_PM:	
	
                bootGetChar(&(src_addr.Val[0]));
		bootGetChar(&(src_addr.Val[1]));
		bootGetChar(&(src_addr.Val[2]));
		src_addr.Val[3]=0;
				
                for(i = 0; i < PM_ROW_SIZE*3; i++){
		    bootGetChar(&(buffer[i]));
		}

		pmErasePage(src_addr.Val32);
                pmWritePage(buffer, src_addr.Val32);

                LED_3 = ~LED_3;

                // Send Acknowledgement 
                buffer[0] = COMMAND_ACK;
                radioTxData(1, buffer, 0, CMD_BOOTLOADER);       
                
                break;

            case COMMAND_READ_GOTO: 
                
                pmReadGoto(buffer);
                radioTxData(6, buffer, 0, CMD_BOOTLOADER);
                break;

	    case COMMAND_READ_ID: // tested

                pmReadId(buffer);
                radioTxData(6, buffer, 0, CMD_BOOTLOADER);
                break;
			
            case COMMAND_GET_VERSION:

                radioTxData(VERSION_LENGTH, VERSION, 0, CMD_BOOTLOADER);
                break;

	    case COMMAND_WRITE_CM:	
                // just read out incoming data, but do not write to program memory
		for(i = 0; i < CM_ROW_SIZE*3;)	{
		    bootGetChar(&(buffer[i++]));
	            bootGetChar(&(buffer[i++]));
		    bootGetChar(&(buffer[i++]));

                    buffer[0] = COMMAND_ACK;
                    radioTxData(1, buffer, 0, CMD_BOOTLOADER);      
		}
                break;
	
            case COMMAND_RESET: 
		
                for(i = 0, src_addr.Val32 = 0xF80000; 
                    i < CM_ROW_SIZE*3;
                    i +=3, src_addr.Val32 += 2) {
		    
                    if(buffer[i] == 0){
			Temp.Val[0]=buffer[i+1];
			Temp.Val[1]=buffer[i+2];
                    
                        // temporary disable writing CM
                        //WriteLatch( src_addr.Word.HW,src_addr.Word.LW,
			//		Temp.Word.HW,Temp.Word.LW);
			//WriteMem(CONFIG_WORD_WRITE);

                    }
		}
			
                return;
		break;
	    
	    case COMMAND_NACK:
                return;
		break;
		
	    default:
                buffer[0] = COMMAND_NACK;
                radioTxData(1, buffer, 0, CMD_BOOTLOADER);      
		break;
	}

    }

}


/******************************************************************************/
void bootReset(void) {

    LED_1 = 0;
    LED_2 = 0;
    LED_3 = 0;
    pmReset();

}


/******************************************************************************/
static void bootGetChar(char *data) {

    while(1) {	
	// if timer expired, signal to application to jump to user code
	if(IFS0bits.T3IF == 1){
	    *data = COMMAND_NACK;
	    break;
	}
        
        if(radioGetChar(data) == 1) {
	    T2CONbits.TON = 0; // Disable timer countdown
            break;
        }
    }
}



/******************************************************************************/
static void bootWriteBuffer(char *data, int size) {

    int start = 0;

    while (size > 0) {
        if (size > 80) {
            radioTxData(80, &data[start], 0, CMD_BOOTLOADER);
            size -= 80;
            start += 80;
        } else {
            radioTxData(size, &data[start], 0, CMD_BOOTLOADER);
            size = 0;
        }
    }

}




