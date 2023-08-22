#include "../i2c-lib/i2c.h"   //Include I2C library
#include "picPWM.h"    //Include PWM library

#include <stdio.h>

int main(){
    i2c I2C;    //Create an instance of the I2C class
<<<<<<< Updated upstream
    int addr = 0b1111111;    //Define the address of the client
=======
    
	int addr = 0b1111111;    //Define the address of the client
    uint8_t duty;
>>>>>>> Stashed changes

    picPWM pwm(addr, I2C);  //Create instance of the picPWM class
    
    /* WRITE PWM INFO TO MCU */
    printf("====================================\n");
    duty = 30;    //Define a duty cycle
    pwm.setMotX(duty);      //Write the duty cycle to Motor X
    printf("Wrote duty cycle 30/200 on MotX\n");

    duty = 60;            //Change duty cycle variable  
    pwm.setMotY(duty);      //Write the duty cycle to Motor Y
    printf("Wrote duty cycle 60/200 on MotY\n");

    duty = 90;            //Change duty cycle variable  
    pwm.setMagX(duty);      //Write the duty cycle to Magnetorquer X
    printf("Wrote duty cycle 90/200 on MagX\n");

    duty = 120;            //Change duty cycle variable  
    pwm.setMagY(duty);      //Write the duty cycle to Magnetorquer Y
    printf("Wrote duty cycle 120/200 on MagY\n");

    duty = 150;            //Change duty cycle variable  
    pwm.setMagZ(duty);      //Write the duty cycle to Magnetorquer Z
    printf("Wrote duty cycle 150/200 on MagZ\n");

    return 0;
}
