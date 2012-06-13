#include "init.h"

unsigned int PTPERvalue; 

void SetupPorts(void)
{
    // Disabling all ADC2 AIO's to enable DIO's
    AD1PCFGL = ENABLE_ALL_DIG_0_15;
    AD2PCFGL = ENABLE_ALL_DIG_0_15;
    
    LATB = 0x0000;
    TRISB = 0x0000;

    // Camera PWDN: RC13-14 are outputs
    LATC = 0x0000;
    TRISC = 0b1001111111111111;

    LATD = 0x0000;
    TRISD = 0b000000000000;

	// LEDs: RF0-1 and RD11(ext int) are outputs
    LATF  = 0b0000000;
    TRISF = 0b0000000;
	// LATD  = 0x000;
    // TRISD = 0b011111111111;
    /* Note that in the above, Data: RD0-7(PIXEL) and Sync:
       RD8-10(VSYNC/HREF/PCLK) remain inputs for the camera */

    // Camera PWDN: RC13-14 are outputs
    // LATC  = 0x0000;
    // TRISC = 0b1001111111111111;

    // Switches: RF2-3 are inputs
    // Batt Supervisor: RF6(ext int) is an input

    /* Extra: RE2(PWM), RB0-3(A/D Conv) all inputs
       or managed thru the peripheral modules */ 

    // dfmem: SPI2 Slave Select is an output
    //LATG  = 0b0000000000;
    //TRISG = 0b0111111111;
}

void SetupInterrupts(void) {
    ConfigINT0(RISING_EDGE_INT & EXT_INT_ENABLE & EXT_INT_PRI_7); // Battery Supervisor
    ConfigIntTimer1(T1_INT_PRIOR_4 & T1_INT_OFF);
    ConfigIntTimer2(T2_INT_PRIOR_6 & T2_INT_OFF);
    ConfigIntTimer3(T3_INT_PRIOR_5 & T3_INT_OFF);
    ConfigIntTimer4(T4_INT_PRIOR_3 & T4_INT_OFF);
}


void SetupUART2(void) {
    /// UART2 for RS-232 w/PC @ 230400, 8bit, No parity, 1 stop bit
    unsigned int U2MODEvalue, U2STAvalue, U2BRGvalue;
    U2MODEvalue = UART_EN & UART_IDLE_CON & UART_IrDA_DISABLE &
                  UART_MODE_FLOW & UART_UEN_10 & UART_DIS_WAKE &
                  UART_DIS_LOOPBACK & UART_DIS_ABAUD & UART_UXRX_IDLE_ONE &
                  UART_BRGH_FOUR & UART_NO_PAR_8BIT & UART_1STOPBIT;
    U2STAvalue  = UART_INT_TX_BUF_EMPTY & UART_SYNC_BREAK_DISABLED &
                  UART_TX_ENABLE & UART_ADR_DETECT_DIS & 
                  UART_IrDA_POL_INV_ZERO; // If not, whole output inverted.

	if (MIPS == 20) {
	    U2BRGvalue  = 21; // (Fcy / ({16|4} * baudrate)) - 1
	} else {
		U2BRGvalue  = 43; // default value for 40 MIPS
	}

    OpenUART2(U2MODEvalue, U2STAvalue, U2BRGvalue);

}


void SetupI2C(unsigned int sclk) {
    unsigned int I2C1CONvalue, I2C1BRGvalue;
    I2C1CONvalue = I2C1_ON & I2C1_IDLE_CON & I2C1_CLK_HLD &
                   I2C1_IPMI_DIS & I2C1_7BIT_ADD & I2C1_SLW_DIS &
                   I2C1_SM_DIS & I2C1_GCALL_DIS & I2C1_STR_DIS &
                   I2C1_NACK & I2C1_ACK_DIS & I2C1_RCV_DIS &
                   I2C1_STOP_DIS & I2C1_RESTART_DIS & I2C1_START_DIS;
	
	if (MIPS == 20) {
		if (sclk == 400) { // 400KHz
			I2C1BRGvalue = 47;  // Fcy(1/Fscl - 1/10000000)-1
		} else {
			I2C1BRGvalue = 197; // default is 100KHz
		}
	} else { // default is 40MIPS
		I2C1BRGvalue = 95;      // 400KHz 
		// I2C1BRGvalue = 363; // for Omnivision, Fcy(1/Fscl - 1/1111111)-1
	}

    OpenI2C1(I2C1CONvalue, I2C1BRGvalue);
    IdleI2C1();
}


void SetupSPI(void) {

    unsigned int SPI2CON1value, SPI2CON2value, SPI2STATvalue;
    SPI2CON1value = ENABLE_SCK_PIN & ENABLE_SDO_PIN & SLAVE_ENABLE_ON &
                    SPI_SMP_ON & SPI_CKE_ON & CLK_POL_ACTIVE_LOW &
                    SPI_MODE16_OFF & MASTER_ENABLE_ON &
                    PRI_PRESCAL_1_1 & SEC_PRESCAL_4_1;
    SPI2CON2value = FRAME_ENABLE_ON & FRAME_SYNC_OUTPUT &
                    FRAME_POL_ACTIVE_HIGH & FRAME_SYNC_EDGE_PRECEDE;
    SPI2STATvalue = SPI_ENABLE & SPI_IDLE_STOP & SPI_RX_OVFLOW_CLR;
    OpenSPI2(SPI2CON1value, SPI2CON2value, SPI2STATvalue);
    //_CS = 1; // De-select mem (slave)
}


void SetupPWM(void) {

    unsigned int SEVTCMPvalue, PTCONvalue, PWMCON1value, PWMCON2value;
    unsigned int PDC2value, PDC3value;

    if (MIPS == 20) {
	    PTPERvalue = 312; // Fcy/(Fpwm * Prescale) - 1
    } else {	
	    PTPERvalue = 624; // default value for 1KHz at MIPS == 40
    }
    
    //SEVTCMPvalue = 620; // Special Event Trigger Compare Value for ADC in phase with PWM
    SEVTCMPvalue = 1; // Special Event Trigger Compare Value for ADC in phase with PWM
    PTCONvalue = PWM_EN & PWM_IDLE_CON & PWM_OP_SCALE1 &
                  PWM_IPCLK_SCALE64 & PWM_MOD_FREE;
    PWMCON1value = PWM_MOD2_IND & PWM_PEN2L & PWM_MOD3_IND & PWM_PEN3L;
    PWMCON2value = PWM_SEVOPS1 & PWM_OSYNC_TCY & PWM_UEN;
    PDC2value = 0;	// 0% duty cycle
    PDC3value = (unsigned int)(2.0 * 100/100.0 * PTPERvalue);   //PWM3 will be inverted

    ConfigIntMCPWM(PWM_INT_DIS & PWM_FLTA_DIS_INT & PWM_FLTB_DIS_INT);
    SetDCMCPWM(2, PDC2value, 0);
    SetDCMCPWM(3, PDC3value, 0);
    OpenMCPWM(PTPERvalue, SEVTCMPvalue, PTCONvalue, PWMCON1value, PWMCON2value);

}



void SetupADC(void){
    unsigned int AD1CON1value, AD1CON2value, AD1CON3value, AD1CON4value;
    unsigned int AD1PCFGHvalue, AD1PCFGLvalue, AD1CSSHvalue, AD1CSSLvalue, AD1CHS0value;

    AD1CON1value = ADC_MODULE_ON & ADC_IDLE_CONTINUE & 
                   ADC_AD12B_10BIT & ADC_FORMAT_INTG & ADC_CLK_MPWM &
                   ADC_MULTIPLE & ADC_AUTO_SAMPLING_ON & ADC_SAMP_ON;
    AD1CON2value = ADC_VREF_EXT_AVSS & ADC_SCAN_OFF & ADC_SELECT_CHAN_0 &
                   ADC_DMA_ADD_INC_1 & ADC_ALT_BUF_OFF & ADC_ALT_INPUT_OFF;
    AD1CON3value = ADC_CONV_CLK_SYSTEM & ADC_CONV_CLK_3Tcy & ADC_SAMPLE_TIME_1;
    AD1CON4value = ADC_DMA_BUF_LOC_1;
    AD1PCFGHvalue = ENABLE_ALL_DIG_16_31;
    AD1PCFGLvalue = ENABLE_AN0_ANA & ENABLE_AN1_ANA & ENABLE_AN2_ANA & ENABLE_AN3_ANA;    
    AD1CSSHvalue = SCAN_NONE_16_31;
    AD1CSSLvalue = 0x000F; // Enabling AN0-3
//    AD1CSSLvalue = 0x0008; // Enabling AN0-3
    DisableIntADC1();
    AD1CHS0value = ADC_CH0_NEG_SAMPLEA_VREFN & ADC_CH0_POS_SAMPLEA_AN3;
    SetChanADC1(0x0000, AD1CHS0value);
    OpenADC1(AD1CON1value, AD1CON2value, AD1CON3value, AD1CON4value, AD1PCFGLvalue, AD1PCFGHvalue, AD1CSSHvalue, AD1CSSLvalue);
}



void SetupTimer1(void) {

    unsigned int T1CON1value, T1PERvalue;

    T1CON1value = T1_ON & T1_SOURCE_INT & T1_PS_1_8 & T1_GATE_OFF & T1_SYNC_EXT_OFF;
    // prescale 1:8
    // Period is set so that period = 0.01s (100Hz), MIPS = 20M
    // value = Fcy/(prescale*Ftimer)
    T1PERvalue = 50000;

    // T1CON1value = T1_OFF & T1_SOURCE_INT & T1_PS_1_1 & T1_GATE_OFF & T1_SYNC_EXT_OFF;
    // prescale 1:1
    // Period is set so that period = 0.00005s (20KHz), MIPS = 40M
    // T1PERvalue = 0x07D0;

    OpenTimer1(T1CON1value, T1PERvalue);	
}


void SetupTimer2(void) {

    unsigned int T2CONvalue, T2PERvalue;

    T2CONvalue = T2_ON & T2_IDLE_STOP & T2_GATE_OFF & T2_PS_1_64 & T2_32BIT_MODE_OFF & T2_SOURCE_INT;
    // prescale 1:64
    //Period is set so that period = 0.01s (100Hz), MIPS = 40M
    T2PERvalue = 6250;

    OpenTimer2(T2CONvalue, T2PERvalue);	
}


void SetupTimer3(void) {

    unsigned int T3CONvalue, T3PERvalue;

    T3CONvalue = T3_ON & T3_IDLE_STOP & T3_GATE_OFF & T3_PS_1_64 & T3_SOURCE_INT;
    // prescale 1:64
    //Period is set so that period = 0.1s (10Hz), MIPS = 20MHz
    T3PERvalue = 31250;

    OpenTimer3(T3CONvalue, T3PERvalue);	
}



void SetupTimer4(void) {

    unsigned int T4CONvalue, T4PERvalue;

    T4CONvalue = T4_ON & T4_IDLE_STOP & T4_GATE_OFF & T4_PS_1_8 & T4_32BIT_MODE_OFF & T4_SOURCE_INT;
    // prescale 1:8
    //Period is set so that period = 0.001s (1000Hz), MIPS = 20MHz
    T4PERvalue = 2500;

    OpenTimer4(T4CONvalue, T4PERvalue);	
}



