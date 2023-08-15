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
    printf("LOG:\tSensing and Estimation called!\n");

    /*----I2C Communication----*/
    int file_i2c;
    int length;
    unsigned char buffer[60] = {0};

    /*Open I2C Bus*/
    char *filename = (char*)"dev/i2c-0"
    if((file_i2c = open(filename, O_RDWR)) < 0){
        printf("ERROR:\tFailed to open I2C bus.\n");
    }

    /*Communicate with IMU*/

    int addrIMU = 0x00;    //Use 'sudo i2cdetect -y 0 to echo addresses of connected devices
    if(ioctl(file_i2c, I2C_SLAVE, addrIMU) < 0){
        printf("ERROR:\tFailed to contact slave at address %d\n", addrIMU);
    }

    /*Read data from IMU*/
    length = 1; //Number of bytes to read
    if(read(file_i2c, buffer,length) != length){
        printf("ERROR:\tFailed to read bytes from slave\n");
    } else {
        //Parse the data recieved into the vairous variables
        printf("LOG:\tGyroscope and Magnometer values updated!\n");
        float gyro[3] = buffer;
        float mag[3] = buffer;
    }

    /*Communicate with MCU*/

    int addrMCU = 0x01;    //Use 'sudo i2cdetect -y 0 to echo addresses of connected devices
    if(ioctl(file_i2c, I2C_SLAVE, addrIMU) < 0){
        printf("ERROR:\tFailed to contact slave at address %d\n", addrMCU);
    }

    /*Read data from MCU*/
    length = 1; //Number of bytes to read
    if(read(file_i2c, buffer,length) != length){
        printf("ERROR:\tFailed to read bytes from slave\n");
    } else {
        //Parse the data recieved into the vairous variables
        float IsunSens[12]; //Assume format: S1I1, S1I2, S1I3, S1I4, S2I1, S2I2, etc.

        /*Convert Currents to x-y position for each sensor in mm*/
        float sunSens[3][2];
        float L = 10.0; //Lx and Ly in mm    

        for(int i = 0; i<3;i++){
            sunSens[i][0] = L * (IsunSens[4*i+1]+IsunSens[4*i+2]-IsunSens[4*i+0]-IsunSens[4*i+3]);
            sunSens[i][0] /= (IsunSens[4*i+0]+IsunSens[4*i+1]-IsunSens[4*i+2]-IsunSens[4*i+3]);
            sunSens[i][1] = L * (IsunSens[4*i+1]+IsunSens[4*i+3]-IsunSens[4*i+0]-IsunSens[4*i+2]);
            sunSens[i][1] /= (IsunSens[4*i+0]+IsunSens[4*i+1]-IsunSens[4*i+2]-IsunSens[4*i+3]);
        }

        printf("LOG:\tSun Sensor values have been updated\n")
        
    }

    /* At this stage, updated values for the Gyroscope, Magnometers and Sun Sensors should be obtained*/

}

void detumble(){
    printf("LOG:\tDetumbling called!\n");
}

void calibration(){
    printf("LOG:\tCalibration called!\n");
}

void readTargetAttitudeCOMMS(){
    printf("LOG:\tRead Target Attitiude (COMMS) called!\n");
}

void readTargetAttitudeIMG(){
    printf("LOG:\tRead Target Attitiude (IMAGER) called!\n");
}

void positionControl(){
    printf("LOG:\tPosition Control called!\n");
}