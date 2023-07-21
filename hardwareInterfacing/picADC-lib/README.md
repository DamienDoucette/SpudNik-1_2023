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
    imu
        Constructor:
            imu(i2c& I2Cobj, uint8_t address);

            Parameters:
                address -   7bit I2C address of the IMU
                I2Cobj  -   Reference to the instance of the i2c class

        Methods:
            update_USER_CTRL(): This is a method to format the configuration options stored in the public USER_CTRL structure of the imu instance
                This data is then written to the IMU to update the USER_CTRL register
                configuration options can be adjusted prior to this command by simple writting to the structure. EX: imu.USER_CTRL.DMP_EN = 1;
   
            update_PWR_MGMT():  This is a method to format the configuration options stored in the public PWR_MGMT structure of the imu instance
                This data is then written to the IMU to update the PWR_MGMT_1 and PWR_MGMT_2 registers
                configuration options can be adjusted prior to this command by simple writting to the structure. EX: imu.PWR_MGMT.SLEEP = 1;

            update_ACCEL_CONFIG():  This is a method to format the configuration options stored in the public ACCEL_CONFIG structure of the imu instance
                This data is then written to the IMU to update the ACCEL_SMPLRT_DIV_1, ACCEL_SMPLRT_DIV_2, ACCEL_INTEL_CTRL, ACCEL_CONFIG_1, ACCEL_CONFIG_2 registers
                configuration options can be adjusted prior to this command by simple writting to the structure. EX: imu.ACCEL_CONFIG.ACCEL_FCHOICE = 1;

            update_GYRO_CONFIG():   This is a method to format the configuration options stored in the public GYRO_CONFIG structure of the imu instance
                This data is then written to the IMU to update the GYRO_SMPLRT_DIV, GYRO_CONFIG_1, GYRO_CONFIG_2 registers
                configuration options can be adjusted prior to this command by simple writting to the structure. EX: imu.GYRO_CONFIG.GYRO_FS_SEL = 1;

            update_MAG_CONFIG():    This is a method to format the configuration options stored in the public MAG_CONFIG structure of the imu instance
                This data is then written to the IMU to update the CNTL2, CNTL3 registers
                configuration options can be adjusted prior to this command by simple writting to the structure. EX: imu.MAG_CONFIG.SRST = 1;
            
            setGyroOffset(int16_t x, int16_t y, int16_t z): This is a method used to set the offset registers for the gyroscope
                Parameters:
                    x   -   16bit value to define the offset of the gyroscope in x
                    y   -   16bit value to define the offset of the gyroscope in y
                    z   -   16bit value to define the offset of the gyroscope in z

            setAccelOffset(int16_t x, int16_t y, int16_t z):    This is a method used to set the offset registers for the accelerometer
                Parameters:
                    x   -   16bit value to define the offset of the accelerometer in x
                    y   -   16bit value to define the offset of the accelerometer in y
                    z   -   16bit value to define the offset of the accelerometer in z

            readAccel(int16_t *x, int16_t *y, int16_t *z):  This is a method used read the three axes of the accelerometer
                Parameters:
                    x   -   Pointer to a 16bit memory location where the x reading is to be stored
                    y   -   Pointer to a 16bit memory location where the y reading is to be stored
                    z   -   Pointer to a 16bit memory location where the z reading is to be stored

                NOTE: When calling this function, it is important to pass a reference (&) to a int16_t variable, not the variable itself
                    EX: int16_t x, y, z;
                        readAccel(&x, &y, &z);

            readGyro(int16_t *x, int16_t *y, int16_t *z):   This is a method used read the three axes of the Gyroscope
                Parameters:
                    x   -   Pointer to a 16bit memory location where the x reading is to be stored
                    y   -   Pointer to a 16bit memory location where the y reading is to be stored
                    z   -   Pointer to a 16bit memory location where the z reading is to be stored

                NOTE: When calling this function, it is important to pass a reference (&) to a int16_t variable, not the variable itself
                    EX: int16_t x, y, z;
                        readGyro(&x, &y, &z);

            readMag(int16_t *x, int16_t *y, int16_t *z):    This is a method used read the three axes of the magnetometer
                Parameters:
                    x   -   Pointer to a 16bit memory location where the x reading is to be stored
                    y   -   Pointer to a 16bit memory location where the y reading is to be stored
                    z   -   Pointer to a 16bit memory location where the z reading is to be stored

                NOTE: When calling this function, it is important to pass a reference (&) to a int16_t variable, not the variable itself
                    EX: int16_t x, y, z;
                        readMag(&x, &y, &z);

            readTemp(int16_t *t):   This is a method used read the three axes of the accelerometer
                Parameters:
                    t   -   Pointer to a 16bit memory location where the x reading is to be stored

                NOTE: When calling this function, it is important to pass a reference (&) to a int16_t variable, not the variable itself
                    EX: int16_t t;
                        readTemp(&t);

            changeBank(uint8_t bank):   This is a method to change the current register bank.  The IMU has 4 register banks and the bank
                needs to be changed to access different output and configuration registers.

                Parameters:
                    bank    -   integer 0-3 corresponding to which register bank is being selected

            whoami():   This is a method to return the ID from the WHOAMI register. This is likely not useful, but was for debugging issues
                and ensuring that reads from the IMU were correct.  The device should always return a value of 0xEA (234)

                Return: Contents of the WHOAMI register which should always be 0xEA (234)

            checkConfig():  This is a method used to read the configuration registers set with the update_XXXX() functions
                This is useful for debugging to ensure that the device is configured as expected
                
                This function will simply read each register and write the value to the terminal so that the user can ensure the
                values are what they expect

Example.cpp
-------------------------------------------------------------------------------------

#include "../i2c-lib/i2c.h"   //Include I2C library
#include "picADC.h"    //Include ADC library

#include <stdio.h>

int main(){
    i2c I2C;  //Create instance of the I2C class
	int addr = 0x68;    //Define address of client

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
