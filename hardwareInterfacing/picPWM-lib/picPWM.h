#ifndef PICPWM_H
#define PICPWM_H

/*Include libraries*/
#include <stdint.h>
#include "../i2c-lib/i2c.h"
#include <unistd.h>

class picPWM {
    /* Class to control the PWM signals for the magnetorquers and reaction wheel motors
        This is specifically designed to interface with the PIC18 MicroChip MCUs

        Note that the various PWM devices will need to be connected to the proper pins on the MCU
        to ensure proper functionality of this software. This software uses the memory addresses 
        of the registers to control each PWM output.  If a different PWM module output is used for a
        given acutator, the address values stored in the various methods will need to be updated to reflect
        this change. It is recommended to leave this as is unless nessecary.
        
        The pin requirements are as follows:
        PIN     |   Actuator        |   PWM output
        =============================================
        RC4     |   Motor X         |   PWM3S1P1
        RC3     |   Motor Y         |   PWM3S1P2
        RC6     |   Magnetorquer X  |   PWM1S1P1
        RC7     |   Magnetorquer Y  |   PWM1S1P2
        RB7     |   Magnetorquer Z  |   PWM2S1P1

        Constructor picPWM(uint8_t address, i2c& I2Cobj):
            Parameters:
                address -   7bit I2C address of the PIC18 MCU
                I2Cobj  -   Reference to the instance of the i2c class

        Methods:
            setMagX:    Method to set the PWM signal for the magnetorquer on the X axis
                Parameters:
                    duty_cycle  -   duty cycle value ranging from 0-199
                    
            setMagY:    Method to set the PWM signal for the magnetorquer on the Y axis
                Parameters:
                    duty_cycle  -   duty cycle value ranging from 0-199

            setMagZ:    Method to set the PWM signal for the magnetorquer on the Z axis
                Parameters:
                    duty_cycle  -   duty cycle value ranging from 0-199

            setMotX:    Method to set the PWM signal for the reaction wheel on the X axis
                Parameters:
                    duty_cycle  -   duty cycle value ranging from 0-199

            setMotY:    Method to set the PWM signal for the reaction wheel on the Y axis
                Parameters:
                    duty_cycle  -   duty cycle value ranging from 0-199
    */
   
    private:
        //Define class variables
        uint8_t addr;
        i2c I2C;

    public:
        //Define methods
        picPWM(uint8_t address, i2c& I2Cobj);

        void setMagX(uint16_t duty_cycle);
        void setMagY(uint16_t duty_cycle);
        void setMagZ(uint16_t duty_cycle);

        void setMotX(uint16_t duty_cycle);
        void setMotY(uint16_t duty_cycle);
};

#endif