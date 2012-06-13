/*********************************
* Name: wiimote.h
* Desc: Header for Wiimote IR Blob detector
* Date: 2009-8-10
* Author: stanbaek
*********************************/

#define FOCAL_LENGTH        1     
// 0.33m apart two LEDs are projected on a camera located 1m away.
// pixel distance is 463 pixels
//
#define BLOB_SIZE_NULL      0xFF

typedef struct {
    int x;
    int y;
    unsigned char size;
} Blob; 


void Wiimote_BasicSetup(void);
void Wiimote_AdvancedSetup(unsigned char sensitivity, unsigned char mode);
void Wiimote_CaptureData(void);
void Wiimote_ConvertBlobs(int index);
Blob* Wiimote_ReadBlobs(void);
unsigned char* Wiimote_GetRawData(void);
void Wiimote_Write( unsigned char subaddr, unsigned char data );
void Wiimote_SendByte( unsigned char byte );
unsigned char Wiimote_ReceiveByte(void);
void Wiimote_NotAcknowledge (void);
void Wiimote_StartTransmission (void);
void Wiimote_EndTransmission (void);


