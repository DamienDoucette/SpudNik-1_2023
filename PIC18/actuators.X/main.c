/*
 * File:   main.c
 * Author: damie
 *
 * Created on June 23, 2023, 11:44 AM
 */


#include <xc.h>

#define _XTAL_FREQ 4000000 // One instruction cycle is 1 microsaec.

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
        
    /*Some function to control the PWM signal to ensure its changeable*/
    for(uint16_t i = 0; i < 0x00C8; i++){
        setPWM(0x048B, i);
        
        __delay_ms(10);
    } 
}

void setPWM(int address, uint8_t duty_cycle){
    
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
        conReg = temp << 4 | 0x0008;    //PWM1CON address is 0x0478
    }
    if(temp == 0x048){
        conReg = temp << 4 | 0x0007;    //PWM1CON address is 0x0487
    }
    
    //Convert address value to a pointer to the register
    int volatile * const pConReg = (int *) conReg;
    *pConReg &= ~(0b10000000);    //Clear the enable bit of the register (Disable PWM)
    
    //Convert address value to a pointer to the register
    int volatile * const pDutyReg = (int *) address;
    *pDutyReg = duty_cycle;     //Set duty cycle register to duty cycle
    *pConReg |= 0b10000000;       //Set the enable bit of the register (Enable PWM)
}

void main(void) {
    PWMsetup();
    while(1){
        loop();
    }
    return;
}