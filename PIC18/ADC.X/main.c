/*
 * File:   Freq_to_I.c
 * Author: krouglicof
 *
 * Created on April 29, 2022, 5:01 PM
 */



/** I N C L U D E S **********************************************************/
#include <xc.h>
#include <stdio.h>						// Standard IO functions
#include <stdlib.h>						// Standard library functions
#include <string.h>

#define _XTAL_FREQ 4000000     // One instruction cycle is 1 microsaec.

uint16_t adc;

void ADCsetup(){
    /*Configure Pin*/
    ADPCH = 0b001101;       //RB5 analog channel
    TRISBbits.TRISB5 = 1;   //Set RB5 to input
    ANSELBbits.ANSELB5 = 1; //Set RB5 to analog
    
//    ADPCH = 0x00;               //RA0 
//    TRISAbits.TRISA0 = 1;   //Set RB5 to input
//    ANSELAbits.ANSELA0 = 1; //Set RB5 to analog
    
    /*Configure ADC*/
    ADCON0bits.FM = 1;   //Right justify
    ADCON0bits.CS = 1;   //ADCRC Clock
    ADACQ = 32;             //Set acquisition time
    ADCON0bits.ON = 1;      //Turn on ADC
}

uint16_t ADCread(){
    /*Read ADC*/
    ADCON0bits.GO = 1;  //Start acquisition
    while(ADCON0bits.GO); //Wait until done
    uint8_t resultHigh = ADRESH;
    uint8_t resultLow = ADRESL;

    /*Convert 2 8bit to 1 16bit*/
    uint16_t result = resultHigh;
    result = result << 8;
    result = result + resultLow;
    
    return result;
}

void loop(){
    while(1){
        adc = ADCread();
        
        __delay_ms(5);
    }
}

void main() {
    ADCsetup();
    loop();
    return;
}