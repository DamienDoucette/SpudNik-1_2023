=====================================================================================
Project Title:          imu-lib
Project Description:    This project is a library to allow for the Raspberry PI to interface
                        with the ICM-20948 IMU over the I2C bus

                        Note that this library does not have functionality for all features of
                        the IMU.  This was created to provide a simple, user friendly method of
                        performing the basic operations of configuring and reading the IMU

Author:                 Damien Doucette
Contact:                dldoucette@upei.ca
                        damien.doucette01@gmail.com
Date:                   14/07/23

=====================================================================================


File Structure:
-------------------------------------------------------------------------------------
    imu-lib
    |-  imu.cpp
    |-  imu.h
    |-  Example.cpp
    |-  README.md
    i2c-lib


Required Libraries:
-------------------------------------------------------------------------------------
External libraries:
    -   stdio.h

Local libraries:
    -   i2c-lib

    NOTE: 'i2c-lib' Folder must be included in the same directory as 'imu-lib'

Classes:
-------------------------------------------------------------------------------------
    imu:
        Constructor:
            imu(i2c& I2Cobj, uint8_t address)

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

            update_ACCEL_CONFIG():  This is a method to format the configuration options stored in the public ACCEL_CONFIG structure of the imu   
                instance
                This data is then written to the IMU to update the ACCEL_SMPLRT_DIV_1, ACCEL_SMPLRT_DIV_2, ACCEL_INTEL_CTRL, ACCEL_CONFIG_1, ACCEL_CONFIG_2 registers
                configuration options can be adjusted prior to this command by simple writting to the structure. EX: imu.ACCEL_CONFIG.ACCEL_FCHOICE = 1;

            update_GYRO_CONFIG():   This is a method to format the configuration options stored in the public GYRO_CONFIG structure of the imu  
                instance
                This data is then written to the IMU to update the GYRO_SMPLRT_DIV, GYRO_CONFIG_1, GYRO_CONFIG_2 registers
                configuration options can be adjusted prior to this command by simple writting to the structure. EX: imu.GYRO_CONFIG.GYRO_FS_SEL = 1;

            update_MAG_CONFIG():    This is a method to format the configuration options stored in the public MAG_CONFIG structure of the imu 
                instance
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
#include "imu.h"
#include "../i2c-lib/i2c.h"

#include <unistd.h>

/*This is an example script to show the basic use of the IMU library
This is designed to interface the ICM 20948 IMU with the Raspberry PI

Note that functionality is limited -- not all functions of the IMU were included in the library.*/

int main(){
    i2c I2C;    //Declare I2C object
    imu IMU(I2C);   //Declare imu object with reference to I2C
    int16_t Ax, Ay, Az, Gx, Gy, Gz, Mx, My, Mz, t;  //Define 16bit variables for each measurement
    
    /*This is an example of how the configuration registers can be modified
    First change the relevant bit to the desired value
    Then call the 'update_XXXXX_CONFIG()' function to send the data to the IMU*/
    IMU.GYRO_CONFIG.GYRO_AVGCFG = 5;    //Set the gyroscope to 32x average
    IMU.update_GYRO_CONFIG();           //Set the updated configuration to the IMU
    
    IMU.checkConfig();  //Useful for debugging -- simply reads the config registers to allow you to see if they are set to what you assigned previously
    
    /*Loop to collect readings*/
    while(1){
    	printf("==========================================\n");
        /*These functions read the accelerometer, gyro, magnetometer and temperature
        The functions take a reference (&) to the variable defined above. This means that the function will write the data directly
        to the variable reference provided. Therefore there is no return from the function*/
        IMU.readAccel(&Ax, &Ay, &Az);
        IMU.readGyro(&Gx, &Gy, &Gz);
        IMU.readMag(&Mx, &My, &Mz);
        IMU.readTemp(&t);

        /*This displays the readings -- Note everything is in raw signed 16bit int meaning that the values range from -32,768 to 32,767*/
        printf("Accelerometer:\nX: %d\tY: %d\tZ: %d\n", Ax, Ay, Az);
        printf("Gyroscope:\nX: %d\tY: %d\tZ: %d\n", Gx, Gy, Gz);
        printf("Magnetometer:\nX: %d\tY: %d\tZ: %d\n", Mx, My, Mz);
        printf("Temperature:\nT: %d\n", t);

        usleep(500000); //Delay 0.5s (500000us)
    }

    I2C.closeBus(); //Close the I2C bus

    return 0;
}