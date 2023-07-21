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
