/** I N C L U D E S **********************************************************/
#include <xc.h>
#include <stdio.h>						// Standard IO functions
#include <stdlib.h>						// Standard library functions
#include <string.h>

#define _XTAL_FREQ 4000000     // One instruction cycle is 1 microsaec.

//This array hold the ADPCH bits for each pin
uint8_t channel_map[12] = {
    0b000101,   //RA5
    0b000100,   //RA4
    0b000011,   //RA3
    0b010101,   //RC5
    0b010100,   //RC4
    0b010011,   //RC3
    0b010110,   //RC6
    0b010111,   //RC7
    0b001111,   //RB7
    0b001110,   //RB6
    0b001101,   //RB5
    0b001100    //RB4
};

void ADCsetup(){
    /*Configure Pin*/
    TRISA = TRISA | 0b00111000; //Set RA3, RA4, RA5 to inputs
    TRISB = TRISB | 0b11110000; //Set RB4, RB5, RB6, RB7 to inputs
    TRISC = TRISC | 0b11111000; //Set RC3, RC4, RC5, RC6, RC7 to inputs
    
    ANSELA = ANSELA | 0b00111000; //Set RA3, RA4, RA5 to analog inputs
    ANSELB = ANSELB | 0b11110000; //Set RB4, RB5, RB6, RB7 to analog inputs
    ANSELC = ANSELC | 0b11111000; //Set RC3, RC4, RC5, RC6, RC7 to analog inputs
    
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
    FVRCONbits.EN = 1;  //Enable FVR module
    
    ADCON0bits.FM = 1;      //Right justify
    ADCON0bits.CS = 1;      //Set clock to ADCRC Clock (~600kHz)
    ADACQ = 32;             //Set acquisition time
}

uint16_t ADCread(int channel){
    /*
     * Channel:
     * 0  -- RA5                 6  -- RC6
     * 1  -- RA4                 7  -- RC7
     * 2  -- RA3                 8  -- RB7
     * 3  -- RC5                 9  -- RB6
     * 4  -- RC4                 10 -- RB5
     * 5  -- RC3                 11 -- RB4
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
    result = result + resultLow;
    
    return result;
}

void loop(){
    
}

void main() {
    ADCsetup();
    loop();
    return;
}
