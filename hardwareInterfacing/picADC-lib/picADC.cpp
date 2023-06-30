#include "picADC.h"

picADC::picADC(uint8_t address, i2c& I2Cobj){
    /*Class constructor*/
    addr = address;
    I2C  = I2Cobj;

}

void picADC::readSunSensors(uint16_t* readings){
    /*
    readSunSensors()    -   Reads the 12 sun sensor values
            Return:
                readings    -   Pointer to the array holding the 12 12bit ADC readings
    */
   uint8_t buffer[24];
   I2C.readBus(addr, 24, buffer); //Read sunsenor data from MCU

    /*Convert the 24 bytes read to be 12 2byte values
    These will correspond to the 12bit ADC readings from each sensor*/
    for(int i = 0; i < 12; i++){
        readings[i] = buffer[2*i] << 8 | buffer[2*i + 1];
    }
}