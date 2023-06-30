/*
 * File:   main.c
 * Author: damie
 *
 * Created on June 20, 2023, 1:00 PM
 */


#include <xc.h>

void configI2C(){
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
    
    //I2C1CON1bits.CSD = 1;       //Disable clock stretching
    
    I2C1CON2bits.ACNT = 0;      //First transmission after address will be loaded to the byte count register
    I2C1CON2bits.GCEN = 1;      //Enable general address call -- will respond to address 0x00
    
    I2C1PIEbits.CNT1IE = 1;     //Enable interrupt when byte count reaches 0
    I2C1PIEbits.ACKTIE = 1;     //Enable interrupt for acknowledge clock stretching
    I2C1PIEbits.WRIE = 1;       //Enable interrupt for receiving data to process and determine ACK or NACK
    I2C1PIEbits.ADRIE = 1;      //Enable interrupt for processing address
    I2C1PIEbits.PCIE = 1;       //Enable interrupt when stop condition detected
    I2C1PIEbits.RSCIE = 1;      //Enable interrupt when restart condition detected
    I2C1PIEbits.SCIE = 1;       //Enable interrupt when start condition detected
    
    I2C1BTOC = 0x06;            //Set clock source to LFINTOSC (32kHz)
    I2C1BTObits.TOREC = 1;      //If timeout, reset module
    I2C1BTObits.TOBY32 = 0;     //Enable 32 prescaling
    I2C1BTObits.TOTIME = 0x23;  //Set timeout time to 35ms (32kHz / 32 prescale * 35 = 35ms)
    
    I2C1ADR0 = 0x11;   //Set 7bit address
    
    /*Config I2C interrupts*/
    INTCON0bits.GIE = 1;    //Enable interrupts
    INTCON0bits.IPEN = 1;   //Enable Interrupt Priorities
    
    
    IPR7bits.I2C1IP = 1;    //Set I2C interrupt to high prio
    PIR7bits.I2C1IF = 0;    //Clear interrupt bit
    PIE7bits.I2C1IE = 1;    //Enable I2C interrupt
    
    
    I2C1CON0bits.EN = 1;    //Enable I2c
}

void i2cStart(){
    /*
     *Address is received from the host
     * Address is compared to the address saved on this client
     * If there is a match, the SMA bit is set by hardware to activate client mode
     * Data bit D is cleared by hardware to indicate last byte was address
     * 
     */
    while(I2C1PIEbits.PCIE == 1 && I2C1PIRbits.PCIF == 0){
        if(I2C1ADB0 == I2C1ADR0){
        I2C1CON1bits.ACKDT = 0; //Set ACK
        
        I2C1CON0bits.CSTR = 0;  //Release clock
        }
        
        /*
         *ACK will be sent over SDA to acknowledge a matching address was received
         *On 9th falling edge, ACKTIF will be set and clock will be held again.
         * Reset CSTR and ACKTIF to release clock and resume
         */


        if(I2C1PIRbits.ACKTIF == 1 && I2C1CON0bits.CSTR == 1){
            I2C1PIRbits.ACKTIF = 0;
            I2C1CON0bits.CSTR = 0;
        }

        /*If ACK was generated, Host will now send byte to client
         *When done, I2C1RXIF, I2C1IF, WRIF, D, and RXBF will be set
         *Data is transfered into I2C1RXB
         * Clock is stretched to allow ACK bit to be determined
         * Because ACNT is enabled, first byte should be sent to the 
         */

        if(I2C1CON0bits.CSTR == 1){
            I2C1CON1bits.ACKDT = 0; //Set ACK
            I2C1CON0bits.CSTR = 0; //Release clock
        }

        /*If the last matching address received ended with a READ command*/
        if(I2C1STAT0bits.R == 1){
            if(I2C1STAT1bits.TXBE == 0){  //If there is something in the buffer
                I2C1STAT1bits.CLRBF = 1;    //Clear the buffer
            }
            //Send all data for ADC channels
            for(int i = 0; i < 12; i++){
                while(I2C1STAT1bits.TXBE == 0); //Wait for buffer to be emptied
                I2C1TXB = 0x00;     //Load LsB of adc to the buffer
                //while(I2C1CON1bits.ACKSTAT == 1);   //Wait for host to acknowledge
                
                while(I2C1STAT1bits.TXBE == 0); //Wait for buffer to be emptied
                I2C1TXB = 0x00;     //Load MsB to of adc the buffer
                //while(I2C1CON1bits.ACKSTAT == 1);   //Wait for host to acknowledge
            }
        }

        /*If the last matching address received ended with a WRITE command*/
        if(I2C1STAT0bits.R == 0){
            while(I2C1STAT1bits.RXBF == 0); //Wait for buffer to fill
            I2C1CON1bits.ACKDT = 1;     //ACK
            uint8_t addrH = I2C1RXB;    //Read receiver buffer
            I2C1STAT1bits.CLRBF = 1;    //Clear buffer
            
            while(I2C1STAT1bits.RXBF == 0); //Wait for buffer to fill
            I2C1CON1bits.ACKDT = 1;     //ACK
            uint8_t addrL = I2C1RXB;    //Read receiver buffer
            I2C1STAT1bits.CLRBF = 1;    //Clear buffer
            
            while(I2C1STAT1bits.RXBF == 0); //Wait for buffer to fill
            I2C1CON1bits.ACKDT = 1;     //ACK
            uint8_t duty_cycle = I2C1RXB;    //Read receiver buffer
            I2C1STAT1bits.CLRBF = 1;    //Clear buffer
            
            //PWMchangeDuty(addrH << 8 | addrL, duty_cycle);
            
        }

        if(I2C1PIRbits.CNTIF == 1){
            //End of communication
            I2C1CON1bits.ACKCNT = 1;    //Mark end of communication
        }
    }  
}

void __interrupt(irq(I2C1)) ISR(void){
    if(I2C1PIEbits.SCIE && I2C1PIRbits.SCIF){
        i2cStart();
    }
    return;
}


void loop(){
    int cnt = 0;
    if(I2C1PIRbits.SCIF == 1){
        i2cStart();
    }
}

void main(void) {
    configI2C();
    while(1){
        loop();
    }
    return;
}
