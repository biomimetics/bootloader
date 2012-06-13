/*********************************
* Name: buffers.h
* Desc: Header for Buffers
* Date: 2009-9-10
* Author: stanbaek
*********************************/

void Bluetooth_PushFIFO(unsigned int length, unsigned char* data);
void Bluetooth_PopFIFO(unsigned int length, unsigned char* data);
unsigned char Bluetooth_IsEmptyFIFO();
void Bluetooth_PopAllFIFO(void);


