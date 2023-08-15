/*
 * File:   main.c
 * Author: damie
 *
 * Created on August 8, 2023, 3:08 PM
 */


#include <xc.h>
#define _XTAL_FREQ 1000000

uint8_t channel_map[4] = {
    0b010011,   //RC3
    0b010110,   //RC6
    0b010111,   //RC7
    0b001111,   //RB7
};

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
    FVRCONbits.ADFVR = 0b11;    /*Configure the output voltage of the FVR
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
    PWM1PR = 2000;    //Number of clock periods in PWM period (Effectively the resolution)
    PWM1CPRE = 0x00;    //Clock pre-scaler (n+1)
    PWM1GIE = 0x00;     //Interrupt register -- Disable/Enable interrupts  
    PWM1CONbits.LD = 1; //Reload the PR, P1, and P2 registers on the next cycle

    
    /*Configure Slices (Slice is one of the dual outputs*/
    PWM1S1CFGbits.POL1 = 0; //Set polarity of output 1, 0 = high true
    PWM1S1CFGbits.POL2 = 0; //Set polarity of output 2, 0 = high true
    
    PWM1S1CFGbits.PPEN = 1; //Disable Push-Pull to alternate outputs
    PWM1S1CFGbits.MODE = 0b001; //Right align
    
    PWM1S1P1 = 0xFFFF;  //Set duty cycle
    PWM1S1P2 = 0xFFFF;  //Set duty cycle
    
    PWM1CONbits.EN = 1; //Enable PWM module
}

void main(void) {
    OSCFREQ = 0b0010;   //Set HF clock to 4MHz
    OSCCON1bits.NOSC = 0b110;   //Set HF to Fosc
    
    PWMsetup();
    ADCsetup();
    
    /*
    uint16_t Vcell[4];
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
    if(index_max != prev_index_max || index_min != prev_index_max){
        RC2PPS = 0x29;
        RB4PPS = 0x29;
        RB5PPS = 0x29;
        RB6PPS = 0x29;
        switch(index_max){
            case 0:
                RC2PPS = 0x0A;  //Configure to PWM1S1P1_OUT
                break;
            case 1:
                RB4PPS = 0x0A;  //Configure to PWM1S1P1_OUT
                break;
            case 2:
                RB5PPS = 0x0A;  //Configure to PWM1S1P1_OUT
                break;
            case 3:
                RB6PPS = 0x0A;  //Configure to PWM1S1P1_OUT
                break;
        }
        switch(index_min){
            case 0:
                RC2PPS = 0x0B;  //Configure to PWM1S1P2_OUT
                break;
            case 1:
                RB4PPS = 0x0B;  //Configure to PWM1S1P2_OUT
                break;
            case 2:
                RB5PPS = 0x0B;  //Configure to PWM1S1P2_OUT
                break;
            case 3:
                RB6PPS = 0x0B;  //Configure to PWM1S1P2_OUT
                break;
        }
    }
    float Vt = (Vcell[0] + Vcell[1] + Vcell[2] + Vcell[3]);    
    */
    
    RB5PPS = 0x0A;
    RB6PPS = 0x0B;
    return;
}
