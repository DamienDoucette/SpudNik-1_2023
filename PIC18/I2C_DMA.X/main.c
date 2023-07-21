/*
 * File:   main.c
 * Author: damie
 *
 * Created on June 20, 2023, 1:00 PM
 */


#include <xc.h>
#include <pic18f15q40.h>

#define _XTAL_FREQ 4000000

void configDMA(){
    uint16_t bufferReg;
    /*Configure DMA for Start condition*/
    DMASELECT = 0x00;   //Configure DMA 0
    
    DMAnCON0bits.EN = 0;    //Disable the DMA
    DMAnCON0bits.SIRQEN = 0;    //Allow for Hardware triggers
    
    DMAnCON1bits.DMODE = 0b01;  //Destination pointer is incremented
    DMAnCON1bits.DSTP = 1;  //Clear SIRQEN when destination counter is reset
    DMAnCON1bits.SMR = 0b00;    //Set the source memory location to SFR/GPR
    DMAnCON1bits.SSTP = 0;  //No not clear SIRQEN when source counter is reset
    
    DMAnCON0bits.EN = 1;
    
    /*Set the source location and size*/
    DMAnSSAU = 0x0;
    DMAnSSAH = 0x02;
    DMAnSSAL = 0x8A;
    
    DMAnSSZH = 0x00;
    DMAnSSZL = 0x01;
    
    /*Set the destination location and size*/
    DMAnDSAH = &bufferReg >> 8;
    DMAnDSAL = &bufferReg && 0xFF;
    
    DMAnDSZH = 0x00;
    DMAnDSZL = 0x02;
    
    /*Set the hardware source to I2C1RX*/
    DMAnSIRQ = 0x38;
    
    /*Configure DMA for Write condition*/
    DMASELECT = 0x01;   //Configure DMA 0
    
    DMAnCON0bits.EN = 0;    //Disable the DMA
    DMAnCON0bits.SIRQEN = 0;    //Allow for Hardware triggers
    
    DMAnCON1bits.DMODE = 0b01;  //Destination pointer is incremented
    DMAnCON1bits.DSTP = 1;  //Clear SIRQEN when destination counter is reset
    DMAnCON1bits.SMR = 0b00;    //Set the source memory location to SFR/GPR
    DMAnCON1bits.SSTP = 0;  //No not clear SIRQEN when source counter is reset
    
    /*Set the source location and size*/
    DMAnSSAU = 0x0;
    DMAnSSAH = 0x02;
    DMAnSSAL = 0x8A;
    
    DMAnSSZH = 0x00;
    DMAnSSZL = 0x01;
    
    /*Set the destination location and size*/
    DMAnDSAH = bufferReg >> 8;
    DMAnDSAL = bufferReg && 0xFF;
    
    DMAnDSZH = 0x00;
    DMAnDSZL = 0x01;
    
    /*Set the hardware source to I2C1RX*/
    DMAnSIRQ = 0x38;
    
    /*Configure DMA for Read condition*/
    DMASELECT = 0x02;   //Configure DMA 0
    
    DMAnCON0bits.EN = 0;    //Disable the DMA
    DMAnCON0bits.SIRQEN = 0;    //Allow for Hardware triggers
    
    DMAnCON1bits.DMODE = 0b01;  //Destination pointer is incremented
    DMAnCON1bits.DSTP = 0;  //Clear SIRQEN when destination counter is reset
    DMAnCON1bits.SMR = 0b00;    //Set the source memory location to SFR/GPR
    DMAnCON1bits.SSTP = 1;  //No not clear SIRQEN when source counter is reset
    
    /*Set the source location and size*/
    DMAnSSAU = 0x0;
    DMAnSSAH = bufferReg >> 8;
    DMAnSSAL = bufferReg && 0xFF;
    
    DMAnSSZH = 0x00;
    DMAnSSZL = 0x02;
    
    /*Set the destination location and size*/
    DMAnDSAH = 0x02;
    DMAnDSAL = 0x8B;
    
    DMAnDSZH = 0x00;
    DMAnDSZL = 0x01;
    
    /*Set the hardware source to I2C1RX*/
    DMAnSIRQ = 0x39;
    
    
}

void configI2C(){
    I2C1CON0bits.EN = 0;    //Disable I2c
    //SCL1 -- RC1
    //SDA1 -- RC0
    
    //Set all pins to digital not analog
    ANSELCbits.ANSELC0 = 0;
    ANSELCbits.ANSELC1 = 0;
    
    /*Configure pins*/
    //Set pins
    TRISCbits.TRISC0 = 0;
    TRISCbits.TRISC1 = 0;
    //Set pins to open drain
    ODCONCbits.ODCC0 = 1;
    ODCONCbits.ODCC1 = 1;
    
    //Set pins to SDA and SCL output
    RC0PPS = 0x22;  //Set pin C0 to SDA
    RC1PPS = 0x21;  //Set pin C1 to SCL
    
    //Set pins to SDA and SCL input
    I2C1SDAPPS = 0b010000;  //Set C0 to SDA
    I2C1SCLPPS = 0b010001; //Set C1 to SCL
    
    /*Config I2C*/
    I2C1CON0bits.MODE = 0b000; //Set to client 7bit w/o masking
    I2C1CON0bits.CSTR = 0;      //Enable clocking (not stretching clock)
    
    I2C1CON1bits.CSD = 1;       //Disable clock stretching
    
    I2C1CON2bits.ACNT = 0;      //First transmission after address will be loaded to the byte count register
    I2C1CON2bits.GCEN = 1;      //Enable general address call -- will respond to address 0x00
    I2C1CON2bits.ABD = 0;
    I2C1CON2bits.SDAHT = 0b00;
    
    I2C1PIEbits.CNT1IE = 1;     //Enable interrupt when byte count reaches 0
    I2C1PIEbits.ACKTIE = 1;     //Enable interrupt for acknowledge clock stretching
    I2C1PIEbits.WRIE = 1;       //Enable interrupt for receiving data to process and determine ACK or NACK
    I2C1PIEbits.ADRIE = 0;      //Enable interrupt for processing address
    I2C1PIEbits.PCIE = 0;       //Disable interrupt when stop condition detected
    I2C1PIEbits.RSCIE = 0;      //Disable interrupt when restart condition detected
    I2C1PIEbits.SCIE = 0;       //Disable interrupt when start condition detected
    
    //Clear stop, start and restart flags
    I2C1PIRbits.PCIF = 0;
    I2C1PIRbits.SCIF = 0;
    I2C1PIRbits.RSCIF = 0;
    
    I2C1BTOC = 0x03;            //Set clock source to LFINTOSC (32kHz)
    I2C1BTObits.TOREC = 1;      //If timeout, reset module
    I2C1BTObits.TOBY32 = 0;     //Enable 32 prescaling
    I2C1BTObits.TOTIME = 0x23;  //Set timeout time to 35ms (32kHz / 32 prescale * 35 = 35ms)
    
    I2C1ADR0 = 0xFF;   //Set 7bit address
    I2C1ADR1 = 0xFF;   //Set 7bit address
    
    I2C1CLK = 0b0011;   //Set the clock to MFINTOSC (500kHz)
    I2C1BAUD = 0x04;    //Freq = I2C1CLK/(BAUD+1) = 500kHz/(4+1) = 100kHz
    
    /*Config I2C interrupts*/
    INTCON0bits.GIE = 1;    //Enable interrupts
    INTCON0bits.IPEN = 1;   //Enable Interrupt Priorities
    
    
    IPR7bits.I2C1IP = 1;    //Set I2C interrupt to high prio
    PIR7bits.I2C1IF = 0;    //Clear interrupt bit
    PIE7bits.I2C1IE = 1;    //Enable I2C interrupt
    
    I2C1PIRbits.PCIF = 0;   //Clear stop flag    
    
    I2C1CON0bits.EN = 1;    //Enable I2c
    
    __delay_ms(10);
    
    I2C1PIEbits.PCIE = 0;       //Enable interrupt when stop condition detected
    I2C1PIEbits.RSCIE = 1;      //Enable interrupt when restart condition detected
    I2C1PIEbits.SCIE = 1;       //Enable interrupt when start condition detected
}

void __interrupt(irq(I2C1)) ISR(void){
    if(I2C1PIRbits.SCIF == 1){
        I2C1PIRbits.SCIF = 0;
        DMASELECT = 0x00;
        DMAnCON0bits.SIRQEN = 1;    //Allow for Hardware triggers
    }
    
    if(I2C1PIRbits.RSCIF == 1){
        I2C1PIRbits.RSCIF = 0;
        DMASELECT = 0x01;
        DMAnCON0bits.SIRQEN = 1;    //Allow for Hardware triggers
        
        DMASELECT = 0x02;
        DMAnCON0bits.SIRQEN = 1;    //Allow for Hardware triggers
    }
    return;
}

void setup(){
    OSCFREQ = 0b0010;   //Set HF clock to 4MHz
    OSCCON1bits.NOSC = 0b110;   //Set HF to Fosc
    
    configI2C();
    configDMA();
}

void loop(){
    I2C1CNTL = 0x04;
}

void main(void) {
    setup();
    while(1){
        loop();
    }
    return;
}