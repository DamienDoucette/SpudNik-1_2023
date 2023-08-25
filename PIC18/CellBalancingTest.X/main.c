/*
 * File:   main.c
 * Author: damie
 *
 * Created on August 8, 2023, 3:08 PM
 */


#include <xc.h>
#include <stdio.h>
#define _XTAL_FREQ 4000000

uint16_t Vcell[4];
uint8_t channel_map[4] = {
    0b010011,   //RC3
    0b010110,   //RC6
    0b010111,   //RC7
    0b001111,   //RB7
};
float Vt;

void ADCsetup(){
    /*Configure Pin*/
    TRISB = TRISB | 0b10000000; //Set RB7 to inputs
    TRISC = TRISC | 0b11001000; //Set RC3, RC6, RC7 to inputs
    
    ANSELB = ANSELB | 0b10000000; //Set RB7 to analog inputs
    ANSELC = ANSELC | 0b11001000; //Set RC3, RC6, RC7 to analog inputs
    
    /*Configure ADC*/
    ADREFbits.NREF = 0;     /*Set negative reference 
                             * 0: Vref- is connected to AVss, 
                             * 1: Vref- is connected to external Vref-*/
    
    ADREFbits.PREF = 0b11;    /*Set positive reference 
                             * 11: Vref+ is connected to internal FVR module
                             * 10: Vref+ is connected to external Vref+
                             * 01: Reserved
                             * 00: Vref+ is connected to VDD*/
    
    /*IF 0b11 was selected above, configure the FVR module*/
    FVRCONbits.ADFVR = 0b10;    /*Configure the output voltage of the FVR
                                 * 11: 4x = 4.096V
                                 * 10: 2x = 2.048V
                                 * 01: 1x = 1.024V
                                 * 00: OFF*/
    FVRCONbits.EN = 1;  //Enable FVR module
    
    ADCON0bits.FM = 1;      //Right justify
    ADCON0bits.CS = 1;      //Set clock to ADCRC Clock (~600kHz)
    ADACQ = 32;             //Set acquisition time
}

uint16_t ADCread(int channel){
    
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

void PWMsetup(){
    //Set pins to output
    TRISCbits.TRISC2 = 0;
    TRISBbits.TRISB4 = 0;
    TRISBbits.TRISB5 = 0;
    TRISBbits.TRISB6 = 0;
    
    PWM1ERS = 0b0000;   //Sets the external reset source to disabled
    PWM1CLK = 0b0010;   //Sets the clock source for PWM - Set to Fosc
    
    /*Set PWM Period*/
    PWM1PR = 6600;    //Number of clock periods in PWM period (Effectively the resolution)
    PWM1CPRE = 0x00;    //Clock pre-scaler (n+1)
    PWM1GIE = 0x00;     //Interrupt register -- Disable/Enable interrupts  
    PWM1CONbits.LD = 1; //Reload the PR, P1, and P2 registers on the next cycle

    
    /*Configure Slices (Slice is one of the dual outputs*/
    PWM1S1CFGbits.POL1 = 0; //Set polarity of output 1, 0 = high true
    PWM1S1CFGbits.POL2 = 0; //Set polarity of output 2, 0 = high true
    
    PWM1S1CFGbits.PPEN = 1; //Disable Push-Pull to alternate outputs
    PWM1S1CFGbits.MODE = 0b001; //Right align
    
    PWM1S1P1 = 6600;  //Set duty cycle
    PWM1S1P2 = 6600;  //Set duty cycle
    
    PWM1CONbits.EN = 1; //Enable PWM module
}

void balanceCells(){
    uint16_t max = 0;
    uint16_t min = 0xFFFF;
    uint8_t index_max = 0;
    uint8_t index_min = 0;
    uint8_t prev_index_max = 0;
    uint8_t prev_index_min = 0;
   
    for(int i = 0; i < 4; i++){
        uint16_t reading = ADCread(i);
        Vcell[i] = reading;
        
        if(reading > max){
            max = reading;
            prev_index_max = index_max;
            index_max = i;
        }
        
        if(reading < min){
            min = reading;
            prev_index_min = index_min;
            index_min = i;
        }
    }
    if(index_max != prev_index_max || index_min != prev_index_min){
        RC2PPS = 0x29;
        RB4PPS = 0x29;
        RB5PPS = 0x29;
        RB6PPS = 0x29;
        
        switch(index_max){
            case 0:
                RB6PPS = 0x0A;  //Configure to PWM1S1P1_OUT
                break;
            case 1:
                RB5PPS = 0x0A;  //Configure to PWM1S1P1_OUT
                break;
            case 2:
                RB4PPS = 0x0A;  //Configure to PWM1S1P1_OUT
                break;
            case 3:
                RC2PPS = 0x0A;  //Configure to PWM1S1P1_OUT
                break;
        }
        switch(index_min){
            case 0:
                RB6PPS = 0x0B;  //Configure to PWM1S1P2_OUT
                break;
            case 1:
                RB5PPS = 0x0B;  //Configure to PWM1S1P2_OUT
                break;
            case 2:
                RB4PPS = 0x0B;  //Configure to PWM1S1P2_OUT
                break;
            case 3:
                RC2PPS = 0x0B;  //Configure to PWM1S1P2_OUT
                break;
        }
    }
}

void TX(char data){
    int timeout = 0;
    while(U1FIFObits.TXBF && timeout < 1000){timeout++;}
    U1TXB = data;    
}

void UARTsetup(){
    OSCFREQ = 0b0010;   //Set HF clock to 4MHz
    OSCCON1bits.NOSC = 0b110;   //Set HF to Fosc
    
    RC0PPS = 0x10;  //Set UART TX to C0
    TRISCbits.TRISC0 = 0;
    ANSELCbits.ANSELC1 = 0;
    U1RXPPS = 0b010001;  //Set UART RX to C1
    
    U1CON0bits.BRGS = 0;    //Set Baud rate generator to normal speed with 16 clocks per bit
    U1CON0bits.MODE = 0b0011;   //Set mode to 9bit even parity
    U1CON0bits.RXEN = 1;        //Enable receive
    U1CON0bits.TXEN = 1;        //Enable transmit
    
    U1CON2bits.RXPOL = 0;       //Regular polarity - 1 = High
    U1CON2bits.TXPOL = 0;       //Regular polarity - 1 = High
    U1CON2bits.STP = 0b00;      //Num stop bits
    
    /* BAUD = (Fosc * (1+BRGS*3))/(16*(BRG-1))
     * 9600 = (4e6 * (1+0*3))/(16*(BRG-1))
     * 9600 = (4e6)/(16*(BRG-1))
     * BRG-1 = 4e6/(16*9600)
     * BRG-1 = 26.0416666
     * BRG = ~27
    */
    
    U1BRGH = 0;
    U1BRGL = 0x19;  //Set BRG to 27 for baud of ~9600 (manually changed to 25 when debugging)
    
    /*Config I2C interrupts*/
    INTCON0bits.GIE = 1;    //Enable interrupts
    INTCON0bits.IPEN = 1;   //Enable Interrupt Priorities
    
    
    PIE4bits.U1IE = 1;
    PIE4bits.U1RXIE = 1;
    IPR4bits.U1RXIP = 1;
    PIR4bits.U1RXIF = 0;
    
    U1RXB = 0x00;
    
    U1CON1bits.ON = 1;  //Turn on UART 
}

void sendData(){
    char msg[64];
    /*Message format: 'Vcell[0], Vcell[1], Vcell[2], Vcell[3], B1Gate, B2Gate, B3Gate, B4Gate, Total voltage (float)'
     *NOTES: 
     *  The Vcell values will be the raw ADC reading, not adjusted to be the true voltage
     *      To convert to the voltage, multiply by VRef+ and divide by 4095
     *      V = Vref+ * Vcell / 4095
     * 
     *  For the gate values, 10, 11 or 41 will be displayed. These are the direct values of the PPS registers
     *      10  -   This is the PWM signal that should be attached to the highest voltage cell
     *      11  -   This is the PWM signal that should be attached to the lowest voltage cell
     *      41  -   This does not correspond to any peripheral and is used to clear the pin
     *              Meaning that no PWM is connected to a pin displaying 41
    */
    sprintf(msg, "%d, %d, %d, %d, %d, %d, %d, %d, %f\n", 
            Vcell[0], Vcell[1], Vcell[2], Vcell[3],
            RB6PPS, RB5PPS, RB4PPS, RC2PPS,
            Vt);
    
    for(int i = 0; i<64; i++){
        TX(msg[i]);
        if(msg[i] == 0x0A){
            break;
        }
    }
    NOP();
}

void main(void) {
    OSCFREQ = 0b0010;   //Set HF clock to 4MHz
    OSCCON1bits.NOSC = 0b110;   //Set HF to Fosc
    
    PWMsetup();
    ADCsetup();
    UARTsetup();
    
    while(1){
        CLRWDT();
        balanceCells();
        Vt = 2.048*(Vcell[0] + Vcell[1] + Vcell[2] + Vcell[3])/4095;
        __delay_ms(100);
        sendData();
    }
    return;
}
