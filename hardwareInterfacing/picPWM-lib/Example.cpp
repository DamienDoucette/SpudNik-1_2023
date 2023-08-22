#include "../i2c-lib/i2c.h"   //Include I2C library
#include "picPWM.h"    //Include PWM library

#include <stdio.h>

int main(){
    i2c I2C;    //Create an instance of the I2C class
    int addr = 0b1111111;    //Define the address of the client

    picPWM pwm(addr, I2C);  //Create instance of the picPWM class
    
    /* WRITE PWM INFO TO MCU */
    uint8_t duty = 0xAA;    //Define a duty cycle
    pwm.setMagX(duty);      //Write the duty cycle to Magnetorquer X

    duty = 0x0F;            //Change duty cycle variable  
    pwm.setMotY(duty);      //Write the duty cycle to Motor Y

    return 0;
}
