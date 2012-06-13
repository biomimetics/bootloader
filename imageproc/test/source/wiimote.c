/*********************************************
* Name: wiimote.c
* Desc: Communication with Wiimote IR Detector (I2C)
* Date: 2009-8-10
* Author: stanbaek
*********************************************/

#include "init.h"


#define WII_ADDR_RD             0xB1    
#define WII_ADDR_WR             0xB0    // 0x58 << 1
#define WII_DATA_WAIT           1000
#define WII_SETUP_DELAY	        10	// 10 is safe...
#define WII_READ_DELAY          1
#define WII_POSTREAD_DELAY      4

#define WII_DATA_LENGTH         1
#define WII_DATA_WIDTH          16

#define BLOB_LENGTH             1

unsigned char WiiData[WII_DATA_LENGTH][WII_DATA_WIDTH+1];
Blob Blobs[BLOB_LENGTH][4];
// The first byte of WiiData must be thrown away.
// 


/************************************************************************
 * Sensitivity Settings
 * [0x02, 0x00, 0x00, 0x71, 0x01, 0x00, PO, 0x00, P1, P2, P3]
 * P0 = MAX blob size (0x62 - 0xC8)
 * P1 = Sensor GAIN, smaller = higher gain
 * P2 = Sensor gain LIMIT, must be less than GAIN 
 * P3 = MIN blob size, (3 - 5)
 * ***********************************************************************/

const unsigned char WiiSensitivity[5][11] = {
    {0x02, 0x00, 0x00, 0x71, 0x01, 0x00, 0x72, 0x00, 0x20, 0x1F, 0x03},
    {0x02, 0x00, 0x00, 0x71, 0x01, 0x00, 0xC8, 0x00, 0x36, 0x35, 0x03},
    {0x02, 0x00, 0x00, 0x71, 0x01, 0x00, 0xAA, 0x00, 0x64, 0x63, 0x03},
    {0x02, 0x00, 0x00, 0x71, 0x01, 0x00, 0x96, 0x00, 0xB4, 0xB3, 0x04},
    {0x02, 0x00, 0x00, 0x71, 0x01, 0x00, 0x96, 0x00, 0xFE, 0xFE, 0x05}};
    

void Wiimote_BasicSetup(void) {

    Wiimote_Write(0x30, 0x01);
    delay_ms(WII_SETUP_DELAY);
    Wiimote_Write(0x30, 0x08);
    delay_ms(WII_SETUP_DELAY);
    Wiimote_Write(0x06, 0x90);
    delay_ms(WII_SETUP_DELAY);
    Wiimote_Write(0x08, 0xC0);
    delay_ms(WII_SETUP_DELAY);
    Wiimote_Write(0x1A, 0x40);
    delay_ms(WII_SETUP_DELAY);
    Wiimote_Write(0x33, 0x33);
    delay_ms(WII_SETUP_DELAY);

}

void Wiimote_AdvancedSetup(unsigned char sensitivity, unsigned char mode) {
//sensitivity: 1 = highest sensitivity, 5 = lowest

    if (sensitivity > 5 || sensitivity < 1) {
        Wiimote_BasicSetup();
        return;
    }

    Wiimote_Write(0x30, 0x01);
    delay_ms(WII_SETUP_DELAY);
    
    Wiimote_StartTransmission();
    Wiimote_SendByte(WII_ADDR_WR);
    Wiimote_SendByte(0x00);
    Wiimote_SendByte(WiiSensitivity[sensitivity][0]);
    Wiimote_SendByte(WiiSensitivity[sensitivity][1]);
    Wiimote_SendByte(WiiSensitivity[sensitivity][2]);
    Wiimote_SendByte(WiiSensitivity[sensitivity][3]);
    Wiimote_SendByte(WiiSensitivity[sensitivity][4]);
    Wiimote_SendByte(WiiSensitivity[sensitivity][5]);
    Wiimote_SendByte(WiiSensitivity[sensitivity][6]);
    Wiimote_EndTransmission();
    delay_ms(WII_SETUP_DELAY);

    Wiimote_StartTransmission();
    Wiimote_SendByte(WII_ADDR_WR);
    Wiimote_SendByte(0x07);
    Wiimote_SendByte(WiiSensitivity[sensitivity][7]);
    Wiimote_SendByte(WiiSensitivity[sensitivity][8]);
    Wiimote_EndTransmission();
    delay_ms(WII_SETUP_DELAY);

    Wiimote_StartTransmission();
    Wiimote_SendByte(WII_ADDR_WR);
    Wiimote_SendByte(0x1A);
    Wiimote_SendByte(WiiSensitivity[sensitivity][9]);
    Wiimote_SendByte(WiiSensitivity[sensitivity][10]);
    Wiimote_EndTransmission();
    delay_ms(WII_SETUP_DELAY);

    Wiimote_Write(0x33, mode);
    delay_ms(WII_SETUP_DELAY);

    Wiimote_Write(0x30, 0x08);
    delay_ms(WII_SETUP_DELAY);

}


void Wiimote_CaptureData(void) {

    Wiimote_StartTransmission();
    Wiimote_SendByte(WII_ADDR_WR);
    Wiimote_SendByte(0x36);
    Wiimote_EndTransmission();
    delay_ms(WII_READ_DELAY);

    Wiimote_StartTransmission();
    Wiimote_SendByte(WII_ADDR_RD);
    MastergetsI2C1(16, WiiData[0], WII_DATA_WAIT);
    Wiimote_EndTransmission();   
    // delay_ms(WII_POSTREAD_DELAY);  // need delay for some reason

}


/***************************************************************
 * Wiimote_ConvertBlobs                                        *  
 * parameters: index - 
 * *************************************************************/
void Wiimote_ConvertBlobs(int index) {

    unsigned char* wiidata;
    unsigned int i;

    wiidata = WiiData[0] + 1;

    for (i = 0; i < 4; i++) {
        Blobs[0][i].x = ((wiidata[i*3+2] & 0x0030) << 4) + wiidata[i*3];
        Blobs[0][i].y = ((wiidata[i*3+2] & 0x00C0) << 2) + wiidata[i*3+1];
        if (Blobs[0][i].y == 1023) {
            Blobs[0][i].size = BLOB_SIZE_NULL;
        } else {
            Blobs[0][i].size = wiidata[i*3+2] & 0x0F;
        }
    }

}


/***************************************************************
 * Wiimote_ReadBlobs                                           *  
 * return: blobs - structure array of size 4                   *
 * *************************************************************/
Blob* Wiimote_ReadBlobs(void) {

    return  Blobs[0];
}


unsigned char* Wiimote_GetRawData(void) {

    return WiiData[0] + 1; 

}




void Wiimote_Write( unsigned char subaddr, unsigned char data ){
    Wiimote_StartTransmission();
    Wiimote_SendByte(WII_ADDR_WR);
    Wiimote_SendByte(subaddr);
    Wiimote_SendByte(data);
    Wiimote_EndTransmission();
}



void Wiimote_SendByte( unsigned char byte ){
    MasterWriteI2C1(byte);
    while(I2C1STATbits.TRSTAT);
    while(I2C1STATbits.ACKSTAT);
}

unsigned char Wiimote_ReceiveByte(void){
    return MasterReadI2C1();
}

void Wiimote_NotAcknowledge (void){
    NotAckI2C1();
    while(I2C1CONbits.ACKEN);
}

void Wiimote_StartTransmission (void){
    StartI2C1();
    while(I2C1CONbits.SEN);
}

void Wiimote_EndTransmission (void){
    StopI2C1();
    while(I2C1CONbits.PEN);
}



