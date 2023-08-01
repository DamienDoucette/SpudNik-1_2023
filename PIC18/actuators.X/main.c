/*
 * File:   main.c
 * Author: damie
 *
 * Created on June 23, 2023, 11:44 AM
 */


#include <xc.h>
uint8_t i2cBuffer[3];
#define _XTAL_FREQ 4000000 // One instruction cycle is 1 microsaec.

void setPWM(int address, uint8_t duty_cycle){
    
    /*Determine address of PWMxCON register to disable and enable PWM*/
    int temp = address >> 4;    //Remove the last character of the HEX address
    int conReg;                 //Create variable to PWMxCON register address
    
    /*Determine the last character of HEX address based on the prior two
     The address of the CON register for each PWM module shares the 12 MSb of the address
     The last 4 bits are specific to each register*/
    if(temp == 0x046){
        conReg = temp << 4 | 0x0009;    //PWM1CON address is 0x0469
    }
    if(temp == 0x047){
        conReg = temp << 4 | 0x0008;    //PWM1CON address is 0x0478
    }
    if(temp == 0x048){
        conReg = temp << 4 | 0x0007;    //PWM1CON address is 0x0487
    }
    
    //Convert address value to a pointer to the register
    int volatile * const pConReg = (int *) conReg;
    *pConReg &= ~(0b10000000);    //Clear the enable bit of the register (Disable PWM)
    
    //Convert address value to a pointer to the register
    int volatile * const pDutyReg = (int *) address;
    *pDutyReg = duty_cycle;     //Set duty cycle register to duty cycle
    *pConReg |= 0b10000000;       //Set the enable bit of the register (Enable PWM)
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
    
    I2C1CON1bits.CSD = 0;       //Disable clock stretching
    
    I2C1CON2bits.ACNT = 0;      //First transmission after address will be loaded to the byte count register
    I2C1CON2bits.GCEN = 1;      //Enable general address call -- will respond to address 0x00
    I2C1CON2bits.ABD = 0;
    I2C1CON2bits.SDAHT = 0b00;
    
    I2C1PIEbits.CNT1IE = 0;     //Enable interrupt when byte count reaches 0
    I2C1PIEbits.ACKTIE = 0;     //Enable interrupt for acknowledge clock stretching
    I2C1PIEbits.WRIE = 0;       //Enable interrupt for receiving data to process and determine ACK or NACK
    I2C1PIEbits.ADRIE = 1;      //Enable interrupt for processing address
    I2C1PIEbits.PCIE = 0;       //Disable interrupt when stop condition detected
    I2C1PIEbits.RSCIE = 0;      //Disable interrupt when restart condition detected
    I2C1PIEbits.SCIE = 1;       //Enable interrupt when start condition detected
    
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

void i2cStart(){
    /*
     *Address is received from the host
     * Address is compared to the address saved on this client
     * If there is a match, the SMA bit is set by hardware to activate client mode
     * Data bit D is cleared by hardware to indicate last byte was address
     */
    
    while(I2C1STAT0bits.SMA == 0){};
   
    if(I2C1STAT0bits.R == 0){
        int index = 0;
        while(I2C1CNTL > 0){
            I2C1CON0bits.CSTR = 0;
            while(I2C1STAT1bits.RXBF == 0);
            i2cBuffer[index] = I2C1RXB;
            I2C1CON1bits.ACKDT = 0;
            I2C1CON0bits.CSTR = 0;
            index++;
        }   
    }
    
    while(I2C1CON1bits.ACKT == 0);
    
    int address = i2cBuffer[0] << 8 | i2cBuffer[1];
    setPWM(address, i2cBuffer[2]);
    
    /*RESET FOR NEXT COMMUNICATION*/
    I2C1CON0bits.EN = 0;
    I2C1PIR = 0x00;
    I2C1CON0bits.EN = 1;
}

void __interrupt(irq(I2C1)) ISR(void){
    if(I2C1PIRbits.SCIF == 1){
        I2C1PIRbits.SCIF = 0;
        i2cStart();
    }
}
void PWMsetup(){
    //Set pins to output
    TRISCbits.TRISC3 = 0;
    TRISCbits.TRISC4 = 0;
    TRISCbits.TRISC6 = 0;
    TRISCbits.TRISC7 = 0;
    TRISBbits.TRISB7 = 0;
    
    /* Configure the motor PWM signals to C3 and C4 */
    RC4PPS = 0x0E;  //Motor X is set to PWM3S1P1
    RC3PPS = 0x0F;  //Motor Y is set to PWM3S1P2
    
    /* Configure the magnetorquers PWM signals to C6, C7, B7*/
    RC6PPS = 0x0A;  //Mag X is set to PWM1S1P1
    RC7PPS = 0x0B;  //Mag Y is set to PWM1S1P2
    RB7PPS = 0x0C;  //Mag Z is set to PWM2S1P1
    
    /*Configure PWM1*/
    PWM1ERS = 0b0000;   //Sets the external reset source to disabled
    PWM1CLK = 0b0010;   //Sets the clock source for PWM - Set to Fosc
    
    PWM1PR = 0x00C7;    //Number of clock periods in PWM period (Effectively the resolution)
    PWM1CPRE = 0x00;    //Clock pre-scaler (n+1)
    PWM1GIE = 0x00;     //Interrupt register -- Disable/Enable interrupts  
    PWM1CONbits.LD = 1; //Reload the PR, P1, and P2 registers on the next cycle

    /*Configure Slices (Slice is one of the dual outputs*/
    PWM1S1CFGbits.POL1 = 0; //Set polarity of output 1, 0 = high true
    PWM1S1CFGbits.POL2 = 0; //Set polarity of output 2, 0 = high true
    
    PWM1S1CFGbits.PPEN = 0; //Disable Push-Pull to alternate outputs
    PWM1S1CFGbits.MODE = 0b001; //Right align
    
    PWM1S1P1 = 0x0000;  //Set duty cycle
    PWM1S1P2 = 0x0000;  //Set duty cycle
    
    /*Configure PWM2*/
    PWM2ERS = 0b0000;   //Sets the external reset source to disabled
    PWM2CLK = 0b0010;   //Sets the clock source for PWM - Set to Fosc
    
    PWM2PR = 0x00C7;    //Number of clock periods in PWM period (Effectively the resolution)
    PWM2CPRE = 0x00;    //Clock pre-scaler (n+1)
    PWM2GIE = 0x00;     //Interrupt register -- Disable/Enable interrupts  
    PWM2CONbits.LD = 1; //Reload the PR, P1, and P2 registers on the next cycle

    /*Configure Slices (Slice is one of the dual outputs*/
    PWM2S1CFGbits.POL1 = 0; //Set polarity of output 1, 0 = high true
    
    PWM1S1CFGbits.PPEN = 0; //Disable Push-Pull to alternate outputs
    PWM1S1CFGbits.MODE = 0b001; //Right align
    
    PWM2S1P1 = 0x0000;  //Set duty cycle
    
    /*Configure PWM3*/
    PWM3ERS = 0b0000;   //Sets the external reset source to disabled
    PWM3CLK = 0b0010;   //Sets the clock source for PWM - Set to Fosc
    
    PWM3PR = 0x00C7;    //Number of clock periods in PWM period (Effectively the resolution)
    PWM3CPRE = 0x00;    //Clock pre-scaler (n+1)
    PWM3GIE = 0x00;     //Interrupt register -- Disable/Enable interrupts  
    PWM3CONbits.LD = 1; //Reload the PR, P1, and P2 registers on the next cycle

    /*Configure Slices (Slice is one of the dual outputs*/
    PWM3S1CFGbits.POL1 = 0; //Set polarity of output 1, 0 = high true
    PWM3S1CFGbits.POL2 = 0; //Set polarity of output 2, 0 = high true
    
    PWM3S1CFGbits.PPEN = 0; //Disable Push-Pull to alternate outputs
    PWM3S1CFGbits.MODE = 0b001; //Right align
    
    PWM3S1P1 = 0x0000;  //Set duty cycle
    PWM3S1P2 = 0x0000;  //Set duty cycle
    
    /*Enable all PWM modules*/
    PWM1CONbits.EN = 1; //Enable PWM1 module
    PWM2CONbits.EN = 1; //Enable PWM2 module
    PWM3CONbits.EN = 1; //Enable PWM3 module
}

void loop(){

}

void main(void) {
    PWMsetup();
    configI2C();
    while(1){
        loop();
    }
    return;
}