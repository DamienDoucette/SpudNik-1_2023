/*
 * File:   main.c
 * Author: damie
 *
 * Created on July 26, 2023, 9:03 AM
 */

#define _XTAL_FREQ 1000000
#include <xc.h>

void setup(){
    __CONFIG(FEXTOSC, 0b110);   //Configure the EXTOSC mode for a 500kHz-8MHz clock
    OSCENbits.EXTOEN = 1;       //Enable the external oscillator
    OSCCON1bits.NOSC = 0b111;   //Switch the clock to EXTOSC
    
    /*Config pin for testing*/
    TRISBbits.TRISB7 = 0;
}

void loop(){
    LATBbits.LATB7 = 1;
    __delay_us(10);
    LATBbits.LATB7 = 0;
    __delay_us(10);
}

void main(void) {
    setup();
    while(1){
        loop();
    }
    return;
}
