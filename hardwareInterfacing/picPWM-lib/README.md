=====================================================================================
Project Title:          picPWM-lib
Project Description:    This project is used to provide a simple interface for setting the duty cycle for the various actuators on the PIC over I2C
Author:                 Damien Doucette
Contact:                dldoucette@upei.ca
                        damien.doucette01@gmail.com
Date:                   29/06/23

=====================================================================================


File Structure:
-------------------------------------------------------------------------------------
    picPWM-lib
    |-  picPWM.cpp
    |-  picPWM.h
    |-  Example.cpp
    |-  README.md
    i2c-lib

Required Libraries:
-------------------------------------------------------------------------------------
External libraries:
    -   stdint.h

Local libraries:
    -   i2c-lib

    NOTE: 'i2c-lib' Folder must be included in the same directory as 'picPWM-lib'

Classes:
-------------------------------------------------------------------------------------
    picPWM    
        Constructor:
            picPWM(uint8_t address, i2c& I2Cobj):
                    Parameters:
                        address -   7bit I2C address of the PIC18 MCU
                        I2Cobj  -   Reference to the instance of the i2c class

        Methods:
            setMagX(uint8_t duty_cycle):    Method to set the PWM signal for the magnetorquer on the X axis
                Parameters:
                    duty_cycle  -   duty cycle value up to 16bit - actual resolution is defined in PIC script
                    
            setMagY(uint8_t duty_cycle):    Method to set the PWM signal for the magnetorquer on the Y axis
                Parameters:
                    duty_cycle  -   duty cycle value up to 16bit - actual resolution is defined in PIC script

            setMagZ(uint8_t duty_cycle):    Method to set the PWM signal for the magnetorquer on the Z axis
                Parameters:
                    duty_cycle  -   duty cycle value up to 16bit - actual resolution is defined in PIC script

            setMotX(uint8_t duty_cycle):    Method to set the PWM signal for the reaction wheel on the X axis
                Parameters:
                    duty_cycle  -   duty cycle value up to 16bit - actual resolution is defined in PIC script

            setMotY(uint8_t duty_cycle):    Method to set the PWM signal for the reaction wheel on the Y axis
                Parameters:
                    duty_cycle  -   duty cycle value up to 16bit - actual resolution is defined in PIC script

Example.cpp
-------------------------------------------------------------------------------------
#include "../i2c-lib/i2c.h"   //Include I2C library
#include "picPWM.h"    //Include PWM library

#include <stdio.h>

int main(){
    i2c I2C;    //Create an instance of the I2C class
	int addr = 0x68;    //Define the address of the client

    picPWM pwm(addr, I2C);  //Create instance of the picPWM class
    
    /* WRITE PWM INFO TO MCU */
    uint8_t duty = 0xAA;    //Define a duty cycle
    pwm.setMagX(duty);      //Write the duty cycle to Magnetorquer X

    duty = 0x0F;            //Change duty cycle variable  
    pwm.setMotY(duty);      //Write the duty cycle to Motor Y

    return 0;
}
