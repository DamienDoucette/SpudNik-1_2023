/**
 * @file acs.cpp
 * @author Damien Doucette
 * @brief This file contains all of the functions/processes that will be called under the ACS thread in 'main.cpp'
 * @version 0.1
 * @date 2023-06-06
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#include "acs.h"

void sensingAndEstimation(char TLE[2][69], float tprev){
    printf("Sensing and Estimation called!\n");

    /*----I2C Communication----*/
    int file_i2c;
    int length;
    unsigned char buffer[60] = {0};

    /*Open I2C Bus*/
    char *filename = (char*)"dev/i2c-0"
    if((file_i2c = open(filename, O_RDWR)) < 0){
        printf("Failed to open I2C bus.\n");
        return;
    }

    /*Communicate with IMU*/

    int addrIMU = 0x00;    //Use 'sudo i2cdetect -y 0 to echo addresses of connected devices
    if(ioctl(file_i2c, I2C_SLAVE, addrIMU) < 0){
        printf("Failed to contact slave at address %d\n", addrIMU);
    }

    /*Read data from IMU*/
    length = 1; //Number of bytes to read
    if(read(file_i2c, buffer,length) != length){
        printf("Failed to read bytes from slave");
    } else {
        //Parse the data recieved into the vairous variables
        float gyro[3] = buffer;
        float mag[3] = buffer;
    }

    /*Communicate with MCU*/

    int addrMCU = 0x01;    //Use 'sudo i2cdetect -y 0 to echo addresses of connected devices
    if(ioctl(file_i2c, I2C_SLAVE, addrIMU) < 0){
        printf("Failed to contact slave at address %d\n", addrMCU);
    }

    /*Read data from MCU*/
    length = 1; //Number of bytes to read
    if(read(file_i2c, buffer,length) != length){
        printf("Failed to read bytes from slave");
    } else {
        //Parse the data recieved into the vairous variables
        float sunSens[12];
    }

}

void detumble(){
    printf("Detumbling called!\n");
}

void calibration(){
    printf("Calibration called!\n");
}

void readTargetAttitudeCOMMS(){
    printf("Read Target Attitiude (COMMS) called!\n");
}

void readTargetAttitudeIMG(){
    printf("Read Target Attitiude (IMAGER) called!\n");
}

void positionControl(){
    printf("Position Control called!\n");
}