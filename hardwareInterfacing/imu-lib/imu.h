/**
 * @file imu.h
 * @author Damien Doucette (dldoucette@upei.ca)
 * @brief Header file for library to interface ICM-20948 IMU with the Raspberry PI over I2C
 * @version 0.1
 * @date 2023-07-14
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef IMU_H
#define IMU_H

#include "../i2c-lib/i2c.h"
#include <stdio.h>

class imu {
    /* Class to interface with the ICM-20948 IMU chip
        Note that all functionality of the ICM-20948 is not included in this class.  The purpose of this class is to
        create a simple interface to allow for the IMU data to be read to the Raspberry PI.

        More advanaced features of the IMU such as the DMP, FIFO, Interrupts and External sensors is not configurable or acccessible
        with this software.

        Constructor imu(i2c& I2Cobj, uint8_t address):
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
    */

    private:
        uint8_t addr;   //i2c address of the IMU
        i2c I2C;        //I2C object    

    public:
        uint8_t currentBank = 0xFF; //Stores the current register bank being accessed

        /*Creates structure 'USER_CTRL' which stores the values for all configuration setting contained in the USER_CTRL register of the IMU*/
        struct {
            bool DMP_EN = 0;        //1 - Enable DMP features   
            bool FIFO_EN = 0;       //1 - Enable FIFO operation
            bool I2C_MST_EN = 0;    //1 - Enable I2C master
            bool I2C_IF_DIS = 0;    //1 - Reset I2C module and place device in SPI mode (NOT USED)
            bool DMP_RST = 0;       //1 - Reset DMP module
            bool SRAM_RST = 0;      //1 - Reset SRAM module
            bool I2C_MST_RST = 0;   //1 - Reset the I2C master module
        } USER_CTRL;

        /*Creates structure 'PWR_MGMT' which stores the values of the configuration setting for the power management registers of the IMU*/
        struct {
            bool DEVICE_RESET = 0;   //1 - Reset the internal registers back to default settings
            bool SLEEP = 0;         //1 - Set device into sleep mode
            bool LP_EN = 0;         //1 - Turn on low power mode
            bool TEMP_DIS = 0;      //1 - Disable temperature sensor
            int CLKSEL = 1;         /*0 - Internal 20MHz clock
                                    1-5 - Auto selects best available clock (RECOMMENDED)
                                      6 - Interal 20MHz clock
                                      7 - Stops the clock*/
            bool DISABLE_ACCEL = 0; //1 - Disable the acceleromter
            bool DISABLE_GYRO = 0;  //1 - Disable the gyroscope
        } PWR_MGMT;

        /*Creates structure 'GYRO_CONFIG' which stores the values of the configuration setting for the gyroscope registers of the IMU*/
        struct{
            uint8_t GYRO_SMPLRT_DIV = 0;    //Gyro sample rate divider - ODR = 1.1kHz/(1+GYRO_SMPLRT_DIV)
            uint8_t GYRO_DLPFCFG = 0;       //Gyro low pass filter configuration
            uint8_t GYRO_FS_SEL = 0;        /*Gyro scale option
                                                00 = +/- 250dps
                                                01 = +/- 500dps
                                                10 = +/- 1000dps
                                                11 = +/- 2000dps*/
            bool GYRO_FCHOICE = 0;          //1 - Enable gyro digital low pass filter

            bool XGYRO_CTEN = 0;        //1 - Enable Gyro X self test
            bool YGYRO_CTEN = 0;        //1 - Enable Gyro Y self test
            bool ZGYRO_CTEN = 0;        //1 - Enable Gyro Z self test
            uint8_t GYRO_AVGCFG = 0;    /*Averaging filter configuration 
                                            0: 1x AVG
                                            1: 2x AVG
                                            2: 4x AVG
                                            3: 8x AVG
                                            4: 16x AVG
                                            5: 32x AVG
                                            6: 64x AVG
                                            7: 128x AVG*/
        } GYRO_CONFIG;

        /*Creates structure 'ACCEL_CONFIG' which stores the values of the configuration setting for the accelerometer registers of the IMU*/
        struct{
            uint16_t ACCEL_SMPLRT_DIV = 0;  //Accel sample rate divider - ODR = 1.125kHz/(1+ACCEL_SMPLRT_DIV)
            uint8_t ACCEL_DLPFCFG = 0;      //Accel low pass filter configuration
            uint8_t ACCEL_FS_SEL = 0;       /*Accel scale select
                                                00: +/- 2g
                                                01: +/- 4g
                                                10: +/- 8g
                                                11: +/- 16g */
            bool ACCEL_FCHOICE = 0;         //1 - Enable accel low pass filter
            bool AX_ST_EN_REG = 0;          //1 - Enable accel x self test
            bool AY_ST_EN_REG = 0;          //1 - Enable accel y self test
            bool AZ_ST_EN_REG = 0;          //1 - Enable accel z self test
            uint8_t DEC3_CFG = 3;           /*Controls number of samples averaged 
                                                0: AVG 1-4 Samples depending on FCHOICE
                                                1: AVG 8 Samples
                                                2: AVG 16 Samples
                                                3: AVG 32 Samples*/

            bool ACCEL_INTEL_EN = 0;        //1 - Enable WOM logic
            bool ACCEL_INTEL_MODE_INT = 1;  /*Set WOM algorithm 
                                                1: Compare with previous sample
                                                0: Initial sample stored and compared to all future samples*/
        } ACCEL_CONFIG;

        /*Creates structure 'MAG_CONFIG' which stores the values of the configuration settings for the magnetometer registers of the IMU*/
        struct{
            uint8_t MODE = 0b00000010;  /*Select operating mode of magnetometer
                                            00000: Power down mode
                                            00001: Single measurement mode
                                            00010: Continuous measurement mode 1
                                            00100: Continuous measurement mode 2
                                            00110: Continuous measurement mode 3
                                            01000: Continuous measurement mode 4
                                            10000: Self-test mode*/
            bool SRST = 0;  //1 - Soft reset of the magnetometer
        } MAG_CONFIG;

        imu(i2c& I2Cobj, uint8_t address = 0b1101000);  //Declare contructor with the I2C address of the IMU set to the default value

        /*Delcare functions required for updating the configuration registers*/
        void update_USER_CTRL();
        void update_PWR_MGMT();
        void update_GYRO_CONFIG();
        void update_ACCEL_CONFIG();
        void update_MAG_CONFIG();

        /*Delcare functions for setting the offset of the accelerometer and gyroscope*/
        void setGyroOffset(int16_t x, int16_t y, int16_t z);
        void setAccelOffset(int16_t x, int16_t y, int16_t z);

        /*Delcare functions for reading the accelerometer, gyroscope, magnetometer and temperature sensor*/
        void readAccel(int16_t *x, int16_t *y, int16_t *z);
        void readGyro(int16_t *x, int16_t *y, int16_t *z);
        void readMag(int16_t *x, int16_t *y, int16_t *z);
        void readTemp(int16_t *t);

        void changeBank(uint8_t bank);  //Delcare function to change the current register bank

        uint8_t whoami();   //Delcare function to return the WHOAMI address from the device

        void checkConfig(); //Delcare function to read the config registers and display their values in the terminal

};

#endif