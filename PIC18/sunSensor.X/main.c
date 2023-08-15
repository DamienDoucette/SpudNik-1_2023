#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define _XTAL_FREQ 4000000

//This array hold the ADPCH bits for each pin
uint8_t channel_map[12] = {
    0b000100,   //RA4
    0b010101,   //RC5
    0b010100,   //RC4
    0b010011,   //RC3
    0b010110,   //RC6
    0b010111,   //RC7
    0b001111,   //RB7
    0b001110,   //RB6
    0b001101,   //RB5
    0b001100,    //RB4
    0b010010,    //RC2
    0b000010    //RA2
};

uint8_t data[24];

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
    I2C1CON0bits.MODE = 0b000; //Set to client 7bit w/o masking
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
    while(I2C1STAT0bits.SMA == 0 && timeout < 10){timeout++;};    //Wait until client mode is active
    timeout = 0;
    if(I2C1STAT0bits.R == 1){   //If a read command was given
        I2C1CON0bits.CSTR = 0;  //Enable the clock (Stop stretching)
        /*For the 12 ADC readings, there are 24 bytes as the ADC is 12 bit*/
        for(int i = 0; i < 24; i ++){
            I2C1TXB = data[i];  //Load data into the transmit buffer
            while(I2C1STAT1bits.TXBE == 0 && timeout < 100){timeout++;}; //Wait until the TX buffer is emptied
            timeout = 0;
        }
    }
    while(I2C1CON1bits.ACKT == 0 && timeout < 5){timeout++;};  //Wait for an ACK to make sure the byte is fully sent
    
    /*RESET FOR NEXT COMMUNICATION*/
    I2C1CON0bits.EN = 0;
    I2C1PIR = 0x00; //Clear the register that holds the flags for conditions
    I2C1CON0bits.EN = 1;
}

void ADCsetup(){
    /*Configure Pin*/
    TRISA = TRISA | 0b00010100; //Set RA2, RA4 to inputs
    TRISB = TRISB | 0b11110000; //Set RB4, RB5, RB6, RB7 to inputs
    TRISC = TRISC | 0b11111100; //Set RC2, RC3, RC4, RC5, RC6, RC7 to inputs
    
    ANSELA = ANSELA | 0b00010100; //Set RA2, RA4 to analog inputs
    ANSELB = ANSELB | 0b11110000; //Set RB4, RB5, RB6, RB7 to analog inputs
    ANSELC = ANSELC | 0b11111100; //Set RC2, RC3, RC4, RC5, RC6, RC7 to analog inputs
    
    /*Configure ADC*/
    ADREFbits.NREF = 0;     /*Set negative reference 
                             * 0: Vref- is connected to AVss, 
                             * 1: Vref- is connected to external Vref-*/
    
    ADREFbits.PREF = 0b00;    /*Set positive reference 
                             * 11: Vref+ is connected to internal FVR module
                             * 10: Vref+ is connected to external Vref+
                             * 01: Reserved
                             * 00: Vref+ is connected to VDD*/
    
    /*IF 0b11 was selected above, configure the FVR module*/
    FVRCONbits.ADFVR = 0b11;    /*Configure the output voltage of the FVR
                                 * 11: 4x = 4.096V
                                 * 10: 2x = 2.048V
                                 * 01: 1x = 1.024V
                                 * 00: OFF*/
    FVRCONbits.EN = 0;  //Enable FVR module
    
    ADCON0bits.FM = 1;      //Right justify
    ADCON0bits.CS = 1;      //Set clock to ADCRC Clock (~600kHz)
    ADACQ = 32;             //Set acquisition time
}

uint16_t ADCread(int channel){
    /*
     * Channel:
     * 0  -- RA4                 6  -- RB7
     * 1  -- RC5                 7  -- RB6
     * 2  -- RC4                 8  -- RB5
     * 3  -- RC3                 9  -- RB4
     * 4  -- RC6                 10 -- RC2
     * 5  -- RC7                 11 -- RA2
     */
    
    /*Set the ADC channel to ground and read to clear charge*/
    ADPCH = 0b111011; 
    ADCON0bits.GO = 1;  //Start acquisition
    while(ADCON0bits.GO); //Wait until done
    
    /*Set channel to selected pin and measure ADC*/
    ADPCH = channel_map[channel];   //Set ADC to pin
    ADCON0bits.ON = 1;      //Turn on ADC
    ADCON0bits.GO = 1;  //Start acquisition
    while(ADCON0bits.GO); //Wait until done
    uint8_t resultHigh = ADRESH;    //Read high register
    uint8_t resultLow = ADRESL;     //Read low register

    /*Convert 2 8bit to 1 16bit*/
    uint16_t result = resultHigh;
    result = result << 8;
    result = result | resultLow;
    
    return result;
}

void __interrupt(irq(I2C1)) ISR(void){
    if(I2C1PIRbits.SCIF == 1){
        I2C1PIRbits.SCIF = 0;
        i2cStart();
    }
}

void loop(){
    I2C1CNTL = 0x19;    //Set count to 24
    uint16_t temp = 0;  //Variable to hold the 12bit ADC reading
    for(int i = 0; i < 12; i++){
        temp = ADCread(i);  //Read the ADC channel
        //Store the data into a global array that will be transmitted when the I2C is called
        data[2*i] = temp >> 8;
        data[2*i+1] = temp & 0xFF;
    }
}

void main() {
    OSCFREQ = 0b0010;   //Set HF clock to 4MHz
    OSCCON1bits.NOSC = 0b110;   //Set HF to Fosc
    
    configI2C();    //Setup the I2C registers
    ADCsetup();     //Setup the ADC registers
    while(1){
        loop();
    }
    return;
}
