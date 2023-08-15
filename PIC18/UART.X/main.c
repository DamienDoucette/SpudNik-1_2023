/*
 * File:   main.c
 * Author: damie
 *
 * Created on July 17, 2023, 9:17 AM
 */


#include <xc.h>
#define _XTAL_FREQ 4000000
uint8_t receive[64];
int count = 0;

void TX(char data){
    int timeout = 0;
    while(U1FIFObits.TXBF && timeout < 1000){timeout++;}
    U1TXB = data;    
}

void setup(){
    OSCFREQ = 0b0010;   //Set HF clock to 4MHz
    OSCCON1bits.NOSC = 0b110;   //Set HF to Fosc
    
//    __CONFIG(FEXTOSC, 0b110);   //Configure the EXTOSC mode for a 500kHz-8MHz clock
//    OSCENbits.EXTOEN = 1;       //Enable the external oscillator
//    OSCCON1bits.NOSC = 0b111;   //Switch the clock to EXTOSC
    
    
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
    
//    U1CON0bits.BRGS = 1;    //Set Baud rate generator to normal speed with 4 clocks per bit
//    U1BRGH = 0;
//    U1BRGL = 0x19;  //Set BRG to 27 for baud of ~9600 (manually changed to 25 when debugging)
    
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

void loop(){
    #define length 13
    char msg[length] = "Testing 1 2 3";
    for(int i = 0; i<length; i++){
        TX(msg[i]);
    }
    TX(0x09);
    
    __delay_ms(1);
}

void __interrupt(irq(U1RX)) ISR(void) {
    int timeout = 0;
    while(~U1FIFObits.RXBF && timeout < 500){timeout++;}
    receive[count] = U1RXB;
    count++;
    
    if(count > 63){
        count = 0;
    }
}


void main(void) {
    setup();
    
    while(1){
        loop();
    }
    return;
}
