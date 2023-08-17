/*
 * File:   main.c
 * Author: damie
 *
 * Created on June 21, 2023, 8:58 AM
 */


#include <xc.h>

#define _XTAL_FREQ 4000000 // One instruction cycle is 1 microsaec.

void PWMsetup(){
    //Set pins to output
    TRISCbits.TRISC7 = 0;   //Set C7 to output
    TRISBbits.TRISB7 = 0;   //Set B6 to output
    
    RB7PPS = 0x0A;  //Configure to PWM1S1P1_OUT
    RC7PPS = 0x0B;  //Configure to PWM1S1P2_OUT
    
    PWM1ERS = 0b0000;   //Sets the external reset source to disabled
    PWM1CLK = 0b0010;   //Sets the clock source for PWM - Set to Fosc
    
    /*Set PWM Period*/
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
    
    PWM1CONbits.EN = 1; //Enable PWM module
}

void loop(){
    /*Create a variable to hold address -- similar to if it was 
     * being received over the I2C interface*/
    int buffer1 = 0x46B;
    int buffer2 = 0x46D;
    
    /*Create a pointer to the memory address for the duty cycle*/
    int volatile * const pwm11 = (int *) buffer1;
    int volatile * const pwm12 = (int *) buffer2;
    
    /*Some function to control the PWM signal to ensure its changeable*/
    for(uint16_t i = 20; i < 0x00C0; i++){
        /*NOTE: PWM must be disabled before changing duty cycle*/
        PWM1CONbits.EN = 0; //Enable PWM module
        *pwm11 = i;                 //Change the duty cycle of P1
        *pwm12 = (0x00C8 - i);    //Change the duty cycle of P2
        
        PWM1CONbits.EN = 1; //Enable PWM module
        
        __delay_ms(10);
    } 
}

void main(void) {
    PWMsetup();
    while(1){
        loop();
    }
    return;
}
