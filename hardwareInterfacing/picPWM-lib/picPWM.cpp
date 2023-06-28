#include "picPWM.h"

void writeToI2C(i2c& I2C, uint8_t addrMCU, uint16_t addrReg, uint8_t dutyCycle){
    
    /*Organize the data into bytes for the I2C write buffer*/
    uint8_t writeBuffer[3];
    writeBuffer[0] = addressReg >> 8;  //Address H
    writeBuffer[1] = addressReg & ~(0b11110000);   //Address L
    writeBuffer[2] = duty_cycle;

    /*Use I2C library to send data to the MCU*/
    if(I2C.writeBus(addrMCU, 3, writeBuffer)){
        printf("ERROR:\tWrite to client %d Failed!", addrMCU);
    }
}

picPWM::picPWM(uint8_t address, i2c& I2Cobj){
    I2C = I2Cobj;
    addr = address;
}

int picPWM::setMagX(uint8_t duty_cycle){
    uint16_t address= 0x46B;   //Memory address for PWM1S1P1 duty cycle
    writeToI2C(I2C, addr, address, duty_cycle);     //Write data to MCU
    return 0;
}

int picPWM::setMagY(uint8_t duty_cycle){
    uint16_t address = 0x46D;   //Memory address for PWM1S1P2 duty cycle
    writeToI2C(I2C, addr, address, duty_cycle); //Write data to MCU
    return 0;
}

int picPWM::setMagZ(uint8_t duty_cycle){
    uint16_t address = 0x47A;   //Memory address for PWM2S1P1 duty cycle
    writeToI2C(I2C, addr, address, duty_cycle); //Write data to MCU
    return 0;
}

int picPWM::setMotX(uint8_t duty_cycle){
    uint16_t address = 0x489;   //Memory address for PWM3S1P1 duty cycle
    writeToI2C(I2C, addr, address, duty_cycle); //Write data to MCU
    return 0;
}

int picPWM::setMotY(uint8_t duty_cycle){
    uint16_t address = 0x48B;   //Memory address for PWM3S1P2 duty cycle
    writeToI2C(I2C, addr, address, duty_cycle); //Write data to MCU
    return 0;
}