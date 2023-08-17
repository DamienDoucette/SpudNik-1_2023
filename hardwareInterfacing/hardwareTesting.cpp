#include "i2c-lib/i2c.h"
#include "imu-lib/imu.h"
#include "picADC-lib/picADC.h"
#include "picPWM-lib/picPWM.h"

i2c I2C;  //Create instance of the I2C class

void testADC(){
    
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
}

void testPWM(){
    int addr = 0b1111111;    //Define the address of the client
    uint8_t duty;

    picPWM pwm(addr, I2C);  //Create instance of the picPWM class
    
    /* WRITE PWM INFO TO MCU */
    printf("====================================\n");
    duty = 30;    //Define a duty cycle
    pwm.setMotX(duty);      //Write the duty cycle to Motor X
    printf("Wrote duty cycle 30/200 on MotX\n");

    duty = 60;            //Change duty cycle variable  
    pwm.setMotY(duty);      //Write the duty cycle to Motor Y
    printf("Wrote duty cycle 60/200 on MotY\n");

    duty = 90;            //Change duty cycle variable  
    pwm.setMagX(duty);      //Write the duty cycle to Magnetorquer X
    printf("Wrote duty cycle 90/200 on MagX\n");

    duty = 120;            //Change duty cycle variable  
    pwm.setMagY(duty);      //Write the duty cycle to Magnetorquer Y
    printf("Wrote duty cycle 120/200 on MagY\n");

    duty = 150;            //Change duty cycle variable  
    pwm.setMagZ(duty);      //Write the duty cycle to Magnetorquer Z
    printf("Wrote duty cycle 150/200 on MagZ\n");
}

void testIMU(){
    imu IMU(I2C);   //Declare imu object with reference to I2C
    int16_t Ax, Ay, Az, Gx, Gy, Gz, Mx, My, Mz, t;  //Define 16bit variables for each measurement
    
    /*This is an example of how the configuration registers can be modified
    First change the relevant bit to the desired value
    Then call the 'update_XXXXX_CONFIG()' function to send the data to the IMU*/
    IMU.GYRO_CONFIG.GYRO_AVGCFG = 5;    //Set the gyroscope to 32x average
    IMU.update_GYRO_CONFIG();           //Set the updated configuration to the IMU
    
    //IMU.checkConfig();  //Useful for debugging -- simply reads the config registers to allow you to see if they are set to what you assigned previously
    
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
}

int main(){

    for(int i = 0; i < 5; i++){
        testADC();
        testPWM();
        testIMU();
    }
    
    I2C.closeBus();
    return 0;
}

