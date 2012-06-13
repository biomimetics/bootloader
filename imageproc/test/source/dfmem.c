/*****************************************************************
* Name: dfmem.c
* Desc: SPI Communication with DataFlash Memory (528 bytes/page)
* Date: 2008-7-23
* Author: fgb
*****************************************************************/

#include "spi.h"
#include "dfmem.h"
#include "dfmem_reg.h"

void MEM_Write (unsigned int buffer, unsigned int byte, unsigned int page, unsigned int length, unsigned int *data)
{
    /* buffer = 1 or 2
     * byte = 0-527 (10 bits)  
     * page = 0-4095 (12 bits)
     * length < 528 - byte
     */
     
    unsigned char command, addr_H, addr_M, addr_L;
    
    MEM_WaitTillReady();

    /// Buffer Write
    
    // Define command dependent on buffer choice
    if (buffer == 1) {command = 0x84;}
    else {command = 0x87;}
    
    // Restructure byte addressing
    // - 14 don't care bits (dcb's) + 10 buffer address bits (which byte)  
    addr_H = 0x00;
    addr_M = page >> 8;
    addr_L = page;
    
    // Write data to specified buffer
    MEM_StartTransmission();

    MEM_SendByte(command);
    MEM_SendByte(addr_H);
    MEM_SendByte(addr_M);
    MEM_SendByte(addr_L);
    
    putsSPI2(length, data);

    MEM_EndTransmission();
    
            
    /// Page Write
    
    // Change command for page writing
    command--;
    
    // Restructure page addressing
    // - 2 dcb's + 12 page address bits (which page) + 10 dcb's
    addr_H = page >> 6;
    addr_M = page << 2;
    addr_L = 0x00;
    
    // Write buffer data to memory page
    MEM_StartTransmission();
    
    MEM_SendByte(command);
    MEM_SendByte(addr_H);
    MEM_SendByte(addr_M);
    MEM_SendByte(addr_L);
    
    MEM_EndTransmission();
}

unsigned int MEM_Read (unsigned int byte, unsigned int page, unsigned int length, unsigned int *data)
{
    /* byte = 0-527 (10 bits)  
     * page = 0-4095 (12 bits)
     * length < 528 - byte
     */
    
    unsigned char addr_H, addr_M, addr_L;
    unsigned int extraLength;
    
    MEM_WaitTillReady();
    
    /// Page read
    
    // Restructure page/byte addressing
    // - 2 dcb's + 12 page address bits (which page) + 10 buffer address bits (which byte)
    addr_H = page >> 6;
    addr_M = byte >> 8 | page << 2;
    addr_L = byte;  
    
    // Read page data from memory page
    MEM_StartTransmission();
    
    MEM_SendByte(0xD2);
    MEM_SendByte(addr_H);
    MEM_SendByte(addr_M);
    MEM_SendByte(addr_L);
    
    // Don't care bytes
    MEM_SendByte(0x00);
    MEM_SendByte(0x00);
    MEM_SendByte(0x00);
    MEM_SendByte(0x00);
    
    extraLength = getsSPI2(length, data, 10);
    
    MEM_EndTransmission();
    
    return extraLength;
}

unsigned char MEM_Ready (void)
{   
//    // Get mem status register
//    MEM_StartTransmission();
//    
//    MEM_SendByte(0xD7);
//    DFSTATUS = MEM_ReceiveByte();
//    
//    MEM_EndTransmission();

    WriteSPI2(0xD7);
    DFSTATUS = ReadSPI2();
    
    // Return ready status
    return  _READY;
    
}

void MEM_WaitTillReady (void)
{   
    // Get mem status register continuously until memory is ready
    MEM_StartTransmission();
    
    MEM_SendByte(0xD7);
    
    //unsigned int i=0;
    
    do {
        DFSTATUS = MEM_ReceiveByte();
        //if(i == 1) {_RF1 = 0;};
        //i++;
    } while (!_READY);
    
    MEM_EndTransmission();
}

void MEM_SendByte (unsigned char byte)
{
    WriteSPI2(byte);
    while(SPI2STATbits.SPITBF);
}

unsigned char MEM_ReceiveByte (void)
{
    WriteSPI2(null);
    return ReadSPI2();
}

void MEM_StartTransmission (void) { _CS = 0; }
void MEM_EndTransmission (void) { _CS = 1; }
