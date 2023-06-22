/*
 * File:   main.c
 * Author: damie
 *
 * Created on June 15, 2023, 2:20 PM
 */

#include <xc.h>
#include <stdio.h>						// Standard IO functions
#include <stdlib.h>						// Standard library functions
#include <string.h>

#define _XTAL_FREQ 4000000     // One instruction cycle is 1 microsaec.

uint16_t duty_cycle;

void PWMsetup(){
    /*Setup Pins*/
    RB6PPS = 0x09;  //Assigns RB6 to CCP1 output
    TRISBbits.TRISB6 = 1;   //Disable output driver
    
    /*Configure PWM*/
    T2PR = 0xFF;            //Set timer pre-scaler for 10bit res
    CCP1CONbits.EN = 1;     //Enable CCP
    CCP1CONbits.FMT = 0;    //Right aligned PWM
    CCP1CONbits.MODE = 0b1100;  //Set mode to PWM
    
    /*Set Duty cycle*/
    duty_cycle = 0x0000;    //Define 16bit Duty cycle (only 10 relevant bits)
    CCPR1 = duty_cycle;     //Write duty cycle to register
    
    PIR3bits.TMR2IF = 0;       //Clear interrupt flag bit
    T2CLKbits.CS = 0b00001;     //Set the timer clock source to Fosc/4
    T2CONbits.CKPS = 0b00;      //Set timer pre-scaler value??
    //T2CONbits.RD16 = 1;       //Supposed to configure to 16bit or 8bit
    T2CONbits.T2ON = 1;
  
    TRISBbits.TRISB6 = 0;
    
    return;
}

void loop(){
    uint16_t duty;
    while(1){
        for(uint16_t i = 0x0000; i < 0x03FF; i++){
            duty_cycle = i;
            duty = i;
            CCPR1 = duty_cycle;
            __delay_ms(30);
        } 
    }
}

void main() {
    PWMsetup();
    loop();
    return;
}