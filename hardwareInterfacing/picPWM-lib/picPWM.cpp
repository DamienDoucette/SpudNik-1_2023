#include "picPWM.h"

void writeToI2C(i2c& I2C, uint8_t addrMCU, uint16_t addrReg, uint8_t dutyCycle){
    
    /*Organize the data into bytes for the I2C write buffer*/
    uint8_t writeBuffer[3];
    writeBuffer[0] = addrReg >> 8;  //Duty cycle register address H
    writeBuffer[1] = addrReg & ~(0xFF00);   //Duty cycle register address L
    writeBuffer[2] = dutyCycle; //Duty cycle value

    /*Use I2C library to send data to the MCU*/
    if(I2C.writeBus(addrMCU, 3, writeBuffer)){
        printf("ERROR:\tWrite to client %d Failed!", addrMCU);
    } else {
        usleep(50); //Delay 50us to allow for the PWM signal to be set before interrupting the PIC
    }
}

picPWM::picPWM(uint8_t address, i2c& I2Cobj){
    I2C = I2Cobj;
    addr = address;
}

void picPWM::setMagX(uint8_t duty_cycle){
    /*
    setMagX:    Method to set the PWM signal for the magnetorquer on the X axis
            Parameters:
                duty_cycle  -   duty cycle value ranging from 0-199
          
    */
    uint16_t address= 0x46B;   //Memory address for PWM1S1P1 duty cycle
    writeToI2C(I2C, addr, address, duty_cycle);     //Write data to MCU
}

void picPWM::setMagY(uint8_t duty_cycle){
    /*
    setMagY:    Method to set the PWM signal for the magnetorquer on the Y axis
            Parameters:
                duty_cycle  -   duty cycle value ranging from 0-199

    */
    uint16_t address = 0x46D;   //Memory address for PWM1S1P2 duty cycle
    writeToI2C(I2C, addr, address, duty_cycle); //Write data to MCU
}

void picPWM::setMagZ(uint8_t duty_cycle){
    /*
    setMagZ:    Method to set the PWM signal for the magnetorquer on the Z axis
            Parameters:
                duty_cycle  -   duty cycle value ranging from 0-199

    */
    uint16_t address = 0x47A;   //Memory address for PWM2S1P1 duty cycle
    writeToI2C(I2C, addr, address, duty_cycle); //Write data to MCU
}

void picPWM::setMotX(uint8_t duty_cycle){
    /*
    setMotX:    Method to set the PWM signal for the reaction wheel on the X axis
            Parameters:
                duty_cycle  -   duty cycle value ranging from 0-199

    */
    uint16_t address = 0x489;   //Memory address for PWM3S1P1 duty cycle
    writeToI2C(I2C, addr, address, duty_cycle); //Write data to MCU
}

void picPWM::setMotY(uint8_t duty_cycle){
    /*
    setMotY:    Method to set the PWM signal for the reaction wheel on the Y axis
            Parameters:
                duty_cycle  -   duty cycle value ranging from 0-199
    */
    uint16_t address = 0x48B;   //Memory address for PWM3S1P2 duty cycle
    writeToI2C(I2C, addr, address, duty_cycle); //Write data to MCU
}