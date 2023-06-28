#include "picADC.h"

picADC::picADC(uint8_t address, i2c& I2Cobj){
    addr = address;
    I2C  = I2Cobj;

}

uint16_t* picADC::readSunSensors(uint8_t address){
    uint16_t readings[12];  //initialize readings variable
    uint8_t *buffer = I2C.readBus(address, 24); //Read sunsenor data from MCU

    /*Convert the 24 bytes read to be 12 2byte values
    These will correspond to the 12bit ADC readings from each sensor*/
    for(int i = 0; i < 12; i++){
        readings[i] = buffer[2*i] << 8 | buffer[2*i + 1];
    }

    return readings;    //return the 12 sunsensor readings
}