=====================================================================================
Project Title:          picADC-lib
Project Description:    This project is used to provide a simple interface for reading sun sensor readings from the PIC over I2C
Author:                 Damien Doucette
Contact:                dldoucette@upei.ca
                        damien.doucette01@gmail.com
Date:                   29/06/23

=====================================================================================


File Structure:
-------------------------------------------------------------------------------------
    picADC-lib
    |-  picADC.cpp
    |-  picADC.h
    |-  Example.cpp
    |-  README.md
    i2c-lib

Required Libraries:
-------------------------------------------------------------------------------------
External libraries:
    -   stdint.h

Local libraries:
    -   i2c-lib

    NOTE: 'i2c-lib' Folder must be included in the same directory as 'picADC-lib'

Classes:
-------------------------------------------------------------------------------------
    picADC
        Constructor:
            picADC(uint8_t address, i2c& I2Cobj)
                Parameters:
                    address -   7bit I2C address of the PIC18 MCU
                    I2Cobj  -   Reference to instance of i2c class 

        Methods:
            readSunSensors(uint16_t* readings)    -   Reads the 12 sun sensor values and stores values to 'readings'
                Parameters:
                    readings    -   Pointer to the location that the ADC readings will be saved

Example.cpp
-------------------------------------------------------------------------------------

#include "../i2c-lib/i2c.h"   //Include I2C library
#include "picADC.h"    //Include ADC library

#include <stdio.h>

int main(){
    i2c I2C;  //Create instance of the I2C class
	int addr = 0b0110000;    //Define address of client

    picADC adc(addr, I2C);  //Create instance of picADC class

    /* READ ADC VALUES */
    uint16_t readings[12];  //Define array to hold the readings values
    adc.readSunSensors(readings);   //Call the function to read the sensors and pass the output array

    /* PRINT VALUES */
    printf("        Sun Sensor Readings         \n");
    printf("====================================\n");

    for(int i = 0; i < 12; i++){
        printf("Reading %d:\t%d\n", i+1, readings[i]);
    }

    printf("====================================\n");
    
    return 0;
}
