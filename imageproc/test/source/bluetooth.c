/*********************************************
* Name: bluetooth.c
* Desc: Communication with Bluetooth (UART)
* Date: 2009-8-10
* Author: stanbaek
*********************************************/

#include "bluetooth.h"
#include "uart.h"

const char *Handshake = "###";
#define STATE_INIT  0
#define STATE_ONE   1
#define STATE_TWO   2
#define STATE_DONE  3

unsigned char Bluetooth_Handshake(void) {

    unsigned char data, done;
    static unsigned char state = STATE_INIT;
    done = 0;

    while(!done){
        data = Bluetooth_Read();

        switch (state) {
            case STATE_INIT:
                if (data == Handshake[0]) {
                    state = STATE_ONE;
                } else {
                    state = STATE_INIT;
                }
                break;

            case STATE_ONE:
                if (data == Handshake[1]) {
                    state = STATE_TWO;
                } else {
                    state = STATE_INIT;
                }                
                break;

            case STATE_TWO:
                if (data == Handshake[2]) {
                    state = STATE_DONE;
                    done = 1;
                } else {
                    state = STATE_INIT;
                }
                break;

            case STATE_DONE:
                break;
            
            default:
                break;
        }
    }

    Bluetooth_Write(Handshake[0]);

    return 1;

}


/* If there is any data, read out and return the last data */
unsigned int Bluetooth_Flushout(void) {
    unsigned int data;

    while(Bluetooth_DataRdy()) {
        data = Bluetooth_Getc();
    }

    return data;
}

/*************************************************************************
* Function Name     : Bluetooth_Busy                                     *
* Description       : This returns status whether the transmission       *  
*                     is in progress or not, by checking Status bit TRMT *
* Parameters        : None                                               *
* Return Value      : Info about whether transmission is in progress.    *
*************************************************************************/
char Bluetooth_Busy(void) {
    return(!U2STAbits.TRMT);
}

/*********************************************************************
* Function Name     : Bluetooth_DataRdy                              *
* Description       : This function checks whether there is any data *
*                     that can be read from the input buffer, by     *
*                     checking URXDA bit                             *
* Parameters        : None                                           *
* Return Value      : char if any data available in buffer           *
*********************************************************************/
char Bluetooth_DataRdy(void) {
    return(U2STAbits.URXDA);
}

/******************************************************************************
* Function Name     : Bluetooth_Getc				                          *			
* Description       : This function reads a byte without checking             *
*                       the status of UART. User must check the status        *
*                       using DataRdyBluetooth() before this function         * 
******************************************************************************/
unsigned int Bluetooth_Getc(void) {
    return ReadUART2();
}


/******************************************************************************
* Function Name     : Bluetooth_Putc				                          *			
* Description       : This function writes a byte. It is strongly recommended *
*                       to put while(BusyBluetooth()); after this function    *
*                       call                                                  * 
******************************************************************************/
void Bluetooth_Putc(unsigned int data) {
    WriteUART2(data);
}


/******************************************************************************
* Function Name     : Bluetooth_PutWord				                          *			
* Description       : This function writes a 16-bit word                      * 
* Parameters        : unsigned int data                                       *
* Return Value      : none                                                    *
******************************************************************************/
void Bluetooth_PutWord(unsigned int data) {

    Bluetooth_Write( data & 0x00ff );
    Bluetooth_Write( (data >> 8) & 0xff);

}

/*********************************************************************
* Function Name     : Bluetooth_Write                                *
* Description       : This function writes data into the bluetooth   *
* Parameters        : unsigned int data the data to be written       *
* Return Value      : None                                           *
*********************************************************************/
void Bluetooth_Write(unsigned int data) {
    WriteUART2(data);
    while(BusyUART2());
}


/***************************************************************************
* Function Name     : Bluetooth_Read                                       *
* Description       : This function returns the contents of UxRXREG buffer *
* Parameters        : None                                                 *  
* Return Value      : unsigned int value from UxRXREG receive buffer       * 
***************************************************************************/
unsigned int Bluetooth_Read(void) {
    while(!DataRdyUART2());
    return ReadUART2();
}

/***************************************************************************
* Function Name     : Bluetooth_PutString                                  *
* Description       : This function puts the data string to be transmitted *
*                     into the transmit buffer (till NULL character or the *
*                     number of byte specified by the 'length' parameter   *
*                     is transmitted)                                      * 
* Parameters        : unsigned int * address of the string buffer to be    *
*                     transmitted                                          *
*                     the length of byte to be transmitted (if length is 0 *
*                     then until NULL character                            *
* Return Value      : None                                                 *  
***************************************************************************/
void Bluetooth_PutString(unsigned int length, unsigned int *buffer) {

	char * temp_ptr = (char *) buffer;

	if (length == 0) {
	    /* transmit till NULL character is encountered */
        while(*temp_ptr != '\0') {
	        while(U2STAbits.UTXBF);  /* wait if the buffer is full */
    	    U2TXREG = *temp_ptr++;   /* transfer data byte to TX reg */
        }
    } else {
        while(length > 0) {
            while(U2STAbits.UTXBF);  /* wait if the buffer is full */
            U2TXREG = *temp_ptr++;   /* transfer data byte to TX reg */
			length--;
        }
	}

	// while(BusyUART2());
}


/******************************************************************************
* Function Name     : Bluetooth_GetString             					      *			
* Description       : This function gets a string of data of specified length * 
*                     if available in the UxRXREG buffer into the buffer      *
*                     specified.                                              *
* Parameters        : unsigned int length the length expected                 *
*                     unsigned int *buffer  the received data to be           * 
*                                  recorded to this array                     *
* Return Value      : unsigned int number of data bytes yet to be received    * 
******************************************************************************/
unsigned int Bluetooth_GetString(unsigned int length, unsigned int *buffer) {

    while(!DataRdyUART2());
    return getsUART2(length, buffer, DataWait);
}




