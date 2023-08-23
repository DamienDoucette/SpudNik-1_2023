#include "../i2c-lib/i2c.h"   //Include I2C library
#include "picPWM.h"    //Include PWM library

#include <stdio.h>

int main(){
    i2c I2C;    //Create an instance of the I2C class
    int addr = 0b1111111;    //Define the address of the client

    uint8_t duty;
    picPWM pwm(addr, I2C);  //Create instance of the picPWM class

    int range1 = 200;    //Duty cycle PWM 1 range set on the PIC chip (Only used for displays)
    int range2 = 200;    //Duty cycle PWM 1 range set on the PIC chip (Only used for displays)
    int range3 = 200;    //Duty cycle PWM 1 range set on the PIC chip (Only used for displays)
    
    /* WRITE PWM INFO TO MCU */
    printf("====================================\n");
    duty = 30;    //Define a duty cycle
    pwm.setMotX(duty);      //Write the duty cycle to Motor X
    printf("Wrote duty cycle %d/%d on MotX\n", duty, range3);

    duty = 60;            //Change duty cycle variable  
    pwm.setMotY(duty);      //Write the duty cycle to Motor Y
    printf("Wrote duty cycle %d/%d on MotY\n", duty, range3);

    duty = 90;            //Change duty cycle variable  
    pwm.setMagX(duty);      //Write the duty cycle to Magnetorquer X
    printf("Wrote duty cycle %d/%d on MagX\n", duty, range1);

    duty = 120;            //Change duty cycle variable  
    pwm.setMagY(duty);      //Write the duty cycle to Magnetorquer Y
    printf("Wrote duty cycle %d/%d on MagY\n", duty, range1);

    duty = 150;            //Change duty cycle variable  
    pwm.setMagZ(duty);      //Write the duty cycle to Magnetorquer Z
    printf("Wrote duty cycle %d/%d on MagZ\n", duty, range2);

    return 0;
}
