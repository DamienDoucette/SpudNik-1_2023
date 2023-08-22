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
