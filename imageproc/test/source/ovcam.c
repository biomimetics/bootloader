/*********************************************
* Name: ovcam.c
* Desc: SCCB Communication with OV7660 (I2C)
* Date: 2007-9-10
* Author: fgb
*********************************************/

#include "ovcam.h"
#include "i2c.h"
#include "delay.h"

void SCCB_SetupOV7660(void)
{
    // SCCB Register Reset
    SCCB_Write(COM7,0x80);
    delay_us(5000);

    // Pixel Clock, Banding Filter, Exposure, Gain
    SCCB_Write(CLKRC,0x81); // PCLK = (XCLK/2)/2
    SCCB_Write(DM_LNL,0x48);
    SCCB_Write(DM_LNH,0x01);
    SCCB_Write(BD50ST,0x62);
    SCCB_Write(BD60ST,0x52);
    SCCB_Write(COM11,0x02); // 60Hz Banding Filter
    SCCB_Write(COM8,0xF2); // Banding Filter ON
    SCCB_Write(AECH,0x00);
    SCCB_Write(GAIN,0x00);
    SCCB_Write(BLUE,0x80);
    SCCB_Write(RED,0x80);
    SCCB_Write(COM8,0xF7); // AGC Gain + AWB + AEC Exposure

    // Format
    SCCB_Write(COM7,0x10); // QVGA & YUV
    SCCB_Write(COM1,0x24); // QQVGA + HREF skip 3/4 rows (every other in QQVGA)
    SCCB_Write(COM3,0x02); // Horizontal no pixel average + VarioPixel Off (extra)
    SCCB_Write(HSTOP,0x4B);
    SCCB_Write(HSTART,0x23);
    SCCB_Write(HREFreg,0xBF);
    SCCB_Write(VSTRT,0x02);
    SCCB_Write(VSTOP,0x3E);
    SCCB_Write(VREF,0x00);

    SCCB_Write(COM5,0x84); // Enable higher frame rates
    SCCB_Write(COM6,0x62); // Reset all timing on format change + BLC input
    SCCB_Write(COM10,0x02); // VSYNC Negative
    SCCB_Write(0x16,0x02);
    SCCB_Write(PSHFT,0x01);
    SCCB_Write(MVFP,0x39); // Mirror On and VFLIP On
    SCCB_Write(0x29,0x3C); // 20 for internal regulator
    SCCB_Write(CHLF,0x00);
    SCCB_Write(ARBLM,0x07);
    SCCB_Write(0x35,0x84);
    SCCB_Write(0x36,0x00);
    SCCB_Write(ACOM,0x13);
    SCCB_Write(OFON,0x43);
    SCCB_Write(TSLB,0x00); // YUYV output
    SCCB_Write(COM12,0x6C);
    SCCB_Write(COM13,0x90); // Gamma used for raw data before interpolation
    SCCB_Write(EDGE,0x29);
    SCCB_Write(COM15,0xC1); // Output Range 00-FF
    SCCB_Write(COM16,0x20); // Edge Enhancement for YUV
    SCCB_Write(DBLV,0x0A);
    SCCB_Write(0xA1,0xC8);

    SCCB_Write(HV,0x40);
    SCCB_Write(0x43,0xF0);
    SCCB_Write(0x44,0x10);
    SCCB_Write(0x45,0x78);
    SCCB_Write(0x46,0xA8);
    SCCB_Write(0x47,0x60);
    SCCB_Write(0x48,0x80);
    SCCB_Write(0x59,0xBA);
    SCCB_Write(0x5A,0x9A);
    SCCB_Write(0x5B,0x22);
    SCCB_Write(0x5C,0xB9);
    SCCB_Write(0x5D,0x9B);
    SCCB_Write(0x5E,0x10);
    SCCB_Write(0x5F,0xE0);
    SCCB_Write(0x60,0x85); // 05 for advanced AWB
    SCCB_Write(0x61,0x60);
    SCCB_Write(0x9F,0x9D);
    SCCB_Write(DSPC2,0xA0);

    // Matrix Coefficients
    SCCB_Write(MTX1,0x66);
    SCCB_Write(MTX2,0x6B);
    SCCB_Write(MTX3,0x05);
    SCCB_Write(MTX4,0x19);
    SCCB_Write(MTX5,0x40);
    SCCB_Write(MTX6,0x59);
    SCCB_Write(MTX7,0x40);
    SCCB_Write(MTX8,0x40);
    SCCB_Write(MTX9,0x40);
    SCCB_Write(MTXS,0x0D);

    SCCB_Write(0x8B,0xCC);
    SCCB_Write(0x8C,0xCC);
    SCCB_Write(0x8D,0xCF);

    // Gamma Curve
    // GSP
    SCCB_Write(0x6C,0x40);
    SCCB_Write(0x6D,0x30);
    SCCB_Write(0x6E,0x4B);
    SCCB_Write(0x6F,0x60);
    SCCB_Write(0x70,0x70);
    SCCB_Write(0x71,0x70);
    SCCB_Write(0x72,0x70);
    SCCB_Write(0x73,0x70);
    SCCB_Write(0x74,0x60);
    SCCB_Write(0x75,0x60);
    SCCB_Write(0x76,0x50);
    SCCB_Write(0x77,0x48);
    SCCB_Write(0x78,0x3A);
    SCCB_Write(0x79,0x2E);
    SCCB_Write(0x7A,0x28);
    SCCB_Write(0x7B,0x22);
    // GST
    SCCB_Write(0x7C,0x04);
    SCCB_Write(0x7D,0x07);
    SCCB_Write(0x7E,0x10);
    SCCB_Write(0x7F,0x28);
    SCCB_Write(0x80,0x36);
    SCCB_Write(0x81,0x44);
    SCCB_Write(0x82,0x52);
    SCCB_Write(0x83,0x60);
    SCCB_Write(0x84,0x6C);
    SCCB_Write(0x85,0x78);
    SCCB_Write(0x86,0x8C);
    SCCB_Write(0x87,0x9E);
    SCCB_Write(0x88,0xBB);
    SCCB_Write(0x89,0xD2);
    SCCB_Write(0x8A,0xE6);

    SCCB_Write(COM9,0x2E); // Automatic Gain Ceiling (AGC) is 4x + Drop frame policy
    SCCB_Write(AEW,0x68);
    SCCB_Write(AEB,0x58);
}

void SCCB_Write( unsigned char subaddr, unsigned char data )
{
    SCCB_StartTransmission();

    SCCB_SendByte(idaddr_w);
    SCCB_SendByte(subaddr);
    SCCB_SendByte(data);

    SCCB_EndTransmission();
}

unsigned char SCCB_Read( unsigned char subaddr )
{
    unsigned char dataReceived;

    SCCB_StartTransmission();

    SCCB_SendByte(idaddr_w);
    SCCB_SendByte(subaddr);

    SCCB_EndTransmission();
    SCCB_StartTransmission();

    SCCB_SendByte(idaddr_r);
    dataReceived = SCCB_ReceiveByte();
    SCCB_NotAcknowledge();

    SCCB_EndTransmission();

    return dataReceived;
}

void SCCB_SendByte( unsigned char byte )
{
    MasterWriteI2C1(byte);
    while(I2C1STATbits.TRSTAT);
    while(I2C1STATbits.ACKSTAT);
}

unsigned char SCCB_ReceiveByte(void)
{
    return MasterReadI2C1();
}

void SCCB_NotAcknowledge (void)
{
    NotAckI2C1();
    while(I2C1CONbits.ACKEN);
}

void SCCB_StartTransmission (void)
{
    StartI2C1();
    while(I2C1CONbits.SEN);
}

void SCCB_EndTransmission (void)
{
    StopI2C1();
    while(I2C1CONbits.PEN);
}
