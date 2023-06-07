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

void sensingAndEstimation(char TLE[3][69], float tprev){
    printf("Sensing and Estimation called!\n");

    /* Read gyroscope values*/

    //I2C communication to IMU

    float gyro[3];

    /* Read sun sensor values*/

    //I2C communication to MCU

    float sunSens[12];

    /*Read Magnetometer values*/

    //I2C communication to IMU

    float mag[3];


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