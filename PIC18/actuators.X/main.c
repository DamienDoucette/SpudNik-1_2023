/*
 * File:   main.c
 * Author: damie
 *
 * Created on June 23, 2023, 11:44 AM
 */


#include <xc.h>
uint8_t i2cBuffer[3];
#define _XTAL_FREQ 4000000 //Define clock speed, needed for __delay

void setPWM(int address, uint8_t duty_cycle){
    /*This function is used to set the PWM duty cycle. 
     * It takes the address of the duty cycle register
     * and the duty cycle value */
    
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
        conReg = temp << 4 | 0x0008;    //PWM2CON address is 0x0478
    }
    if(temp == 0x048){
        conReg = temp << 4 | 0x0007;    //PWM3CON address is 0x0487
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
 
    /*Configure pins*/
    //Set pins
    TRISCbits.TRISC0 = 0;
    TRISCbits.TRISC1 = 0;
    
    //Set pins to digital not analog
    ANSELCbits.ANSELC0 = 0;
    ANSELCbits.ANSELC1 = 0;
    
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
    I2C1CON0bits.MODE = 0b000;  //Set to client 7bit w/o masking
    I2C1CON0bits.CSTR = 0;      //Enable clocking (not stretching clock)
    
    I2C1CON1bits.CSD = 0;       //Enable clock stretching
    
    I2C1CON2bits.ACNT = 0;      //Disable autoload CNT register
    I2C1CON2bits.GCEN = 1;      //Enable general address call -- will respond to address 0x00
    I2C1CON2bits.ABD = 0;       //Enable the address buffer
    I2C1CON2bits.SDAHT = 0b00;  //Sets minimum hold time on SDA after falling edge of SCL
    
    I2C1PIEbits.CNT1IE = 0;     //Disable interrupt & clock stretching when byte count reaches 0
    I2C1PIEbits.ACKTIE = 0;     //Disable interrupt & clock stretching for acknowledge clock stretching
    I2C1PIEbits.WRIE = 0;       //Disable interrupt & clock stretching for receiving data to process and determine ACK or NACK
    I2C1PIEbits.ADRIE = 1;      //Enable interrupt & clock stretching for processing address
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
    /*This function gets called by the ISR when the start condition is detected*/
    int timeout = 0;
    /*Wait for device to process address and determine if it should be in client
     mode. The timeout will prevent the device from getting stuck in this while
     loop if a different device was called*/
    while(I2C1STAT0bits.SMA == 0 && timeout < 10){timeout++;};
    timeout = 0;
    if(I2C1STAT0bits.R == 0){   //If the address had a write command
        int index = 0;
        while(I2C1CNTL > 0){
            I2C1CON0bits.CSTR = 0;  //Enable clocking (release clock)
            //Wait for the RXB to fill
            while(I2C1STAT1bits.RXBF == 0 && timeout < 100){timeout++;};
            i2cBuffer[index] = I2C1RXB; //Store the value from the RXB buffer
            I2C1CON1bits.ACKDT = 0;     //Send an ACK bit
            index++;
        }   
    }
    
    //Combine the H and L address values, then set the PWM signal
    int address = i2cBuffer[0] << 8 | i2cBuffer[1];
    setPWM(address, i2cBuffer[2]);
    
    /*RESET FOR NEXT COMMUNICATION*/
    I2C1CON0bits.EN = 0;
    I2C1PIR = 0x00;
    I2C1CON0bits.EN = 1;
}

void __interrupt(irq(I2C1)) ISR(void){
    /*ISR tied to the I2C1 interrupt flag
     The only thing set to enable the interrupt is the start condition and address
     */
    
    //If the interrupt was set by a start condition
    if(I2C1PIRbits.SCIF == 1){
        I2C1PIRbits.SCIF = 0;   //Clear the start condition flag
        i2cStart(); //Call the I2C function
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
    I2C1CNTL = 0x03;    //Set the CNT register to 3, as there are three bytes of data in a transmission
}

void main(void) {
    PWMsetup(); //Setup the PWM registers
    configI2C();    //Setup the I2C registers
    while(1){
        loop();
    }
    return;
}