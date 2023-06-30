#ifndef PICADC_H
#define PICADC_H

#include <stdint.h>
#include "../i2c-lib/i2c.h"

class picADC{
    /*
        Class to read the ADC values from the sun sensors

        Constructor picADC(uint8_t address, i2c& I2Cobj)
            Parameters:
                address -   7bit I2C address of the PIC18 MCU
                I2Cobj  -   Reference to instance of i2c class 

        Methods:
            readSunSensors(uint16_t* readings)    -   Reads the 12 sun sensor values and stores values to 'readings'
                Parameters:
                    readings    -   Pointer to the location that the ADC readings will be saved

    */

    private:
        //Declare class variables
        uint8_t addr;
        i2c I2C;

    public:
        //Declare methods
        picADC(uint8_t address, i2c& I2Cobj);

        void readSunSensors(uint16_t* readings);
};
#endif