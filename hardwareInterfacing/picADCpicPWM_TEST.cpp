#include "i2c-lib/i2c.h"   //Include I2C library
#include "picADC-lib/picADC.h"    //Include ADC library
#include "picPWM-lib/picPWM.h"    //Include PWM library

#include <stdio.h>

int main(){
    i2c I2C("/dev/i2c-1");
	int addr = 0x68;

    picADC adc(addr, I2C);
    picPWM pwm(addr, I2C);

    /* READ ADC VALUES */
    uint16_t readings[12];  //Define array to hold the readings values
    adc.readSunSensors(readings);   //Call the function to read the sensors and pass the output array
    printf("        Sun Sensor Readings         \n");
    printf("====================================\n");

    for(int i = 0; i < 12; i++){
        printf("Reading %d:\t%d\n", i+1, readings[i]);
    }

    printf("====================================\n");

    /* WRITE PWM INFO TO MCU */
    uint8_t duty = 0xAA;
    pwm.setMagX(duty);

    return 0;
}