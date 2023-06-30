=====================================================================================
Project Title:          i2c-lib
Project Description:    This project is used to provide a simple interface for I2C commands on the Raspberry PI running Ubuntu OS
Author:                 Damien Doucette
Contact:                dldoucette@upei.ca
                        damien.doucette01@gmail.com
Date:                   29/06/23

=====================================================================================


File Structure:
-------------------------------------------------------------------------------------
    i2c-lib
    |-  i2c.cpp
    |-  i2c.h
    |-  Example.cpp
    |-  README.md


Required Libraries:
-------------------------------------------------------------------------------------
External libraries:
    -   unistd.h
    -   fcntl.h
    -   sys/ioctl.h
    -   linux/i2c-dev.h
    -   stdio.h
    -   cstdint

Local libraries:

Classes:
-------------------------------------------------------------------------------------
    i2c:
        Constructor:
            i2c(const char* i2cBus = "/dev/i2c-1")

            Parameters:
                i2cBus  -   Name of the I2C bus file that will be used. A default value of "/dev/i2c-1" is set and has been tested with the Raspberry PI 4

        Methods:
            readBus(uint8_t address, int length, uint8_t buffer[n]) - This method is used to read from a client on the I2C bus.
                Parameters:     
                    address     -   7bit address of the client to read data from
                    length      -   Length of the transmission in bytes
                    buffer      -   Pointer to the memory address to write readings to

                Return:
                    0           -   The transmission was successful
                    ~0          -   An error occured

            writeBus(uint8_t address, int length, uint8_t buffer[n]) - This method is to write to a client on the I2C bus
                Parameters:
                    address     -   7bit address of the client to read data from
                    length      -   Length of the transmission in bytes
                    buffer      -   Pointer to array containing the bytes to be written to the bus. Index 0 of the array will be the first byte transmitted

                Return:
                    0           -   The transmission was successful
                    ~0          -   An error occured

            closeBus() - This method is to close the I2C bus

Example.cpp
-------------------------------------------------------------------------------------
#include "i2c.h"

int main(){
    i2c I2C;    //Create instance of class
    uint8_t addr = 0x68;    //Declare address of the client

    /* READ BYTES */
    int length = 24;        //Delcare the length of the transmission
    uint8_t readBuffer[length];     //Create an array to store the readings

    I2C.readBus(addr, length, readBuffer);  //Read the bus and store data to the readBuffer array

    /* WRITE BYTES */
    length = 3;     //Define length of transmission
    uint8_t writeBuffer[length] = {0xAA, 0xAA, 0xAA};    //Declare and set write buffer

    I2C.writeBus(addr, length, writeBuffer);    //Write the writeBuffer data to the bus

    I2C.closeBus(); //Close the I2C bus

    return 0;
}






