#include "i2c.h"

int main(){
    i2c I2C;    //Create instance of class
    uint8_t addr = 0x68;    //Declare address of the client

    /* READ BYTES */
    int length = 1;        //Delcare the length of the transmission
    uint8_t readBuffer[length];     //Create an array to store the readings

    I2C.readBus(addr, length, readBuffer);  //Read the bus and store data to the readBuffer array

    /* WRITE BYTES */
    length = 1;     //Define length of transmission
    uint8_t writeBuffer[length] = {0xAA};    //Declare and set write buffer

    I2C.writeBus(addr, length, writeBuffer);    //Write the writeBuffer data to the bus

    I2C.closeBus(); //Close the I2C bus

    return 0;
}