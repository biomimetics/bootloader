/*********************************************
* Name: buffers.c
* Desc: Buffers (FIFOs)
* Date: 2009-9-10
* Author: stanbaek
*********************************************/

#define BUFFER_SIZE	    16
#define BUFFER_WIDTH      100

unsigned char Buffer[BUFFER_SIZE][BUFFER_WIDTH];
unsigned int FirstIndex = 0, LastIndex = 0;

void Buffer_Push(unsigned int length, unsigned char* data) {

    unsigned int index;
    unsigned int i;

    index = (LastIndex + 1) & 0x0F;

    if (length == 0) {
        i = 0;
        while(data[i] != '\n') {
            FIFO[index][i] = data[i];
            i++;
        }
        FIFO[index][i] = '\n';

    } else {
        for (i = 0; i < length; i++) {
            FIFO[index][i] = data[i];
        }
    }

    LastIndex = index;

}

void Buffer_Pop(unsigned int length, unsigned char* data) {

    unsigned int i;

    if (length == 0) {
        i = 0;
        while((data[i] = FIFO[FirstIndex][i]) != '\n') {
            i++;
        }
    } else {
        for (i = 0; i < length; i++) {
            data[i] = FIFO[FirstIndex][i];
        }
    }

    if (FirstIndex == 0) {
        FirstIndex = 15;
    } else {
        FirstIndex--;
    }

}

unsigned char Buffer_IsEmpty() {
    
    return (FirstIndex == LastIndex);

}

void Buffer_PopAll(void) {

    unsigned char *data;

    while(FirstIndex != LastIndex) {
        Bluetooth_PopFIFO(0, data);
        Bluetooth_PutString(0, data);
    }

}


