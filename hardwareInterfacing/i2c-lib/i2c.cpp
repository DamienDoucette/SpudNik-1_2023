#include "i2c.h"

i2c::i2c(const char* i2cBus){
    /* Constructor
        Parameter:
            i2cBus  -   Name of the file used to read and write to the I2C bus. 
                        This parameter has a default value of "/dev/i2c-1"
    */

    char *filename = (char*)i2cBus; //Declare pointer to the I2C bus file

    /*Open I2C Bus*/
    if((file_i2c = open(filename, O_RDWR)) < 0){
        printf("ERROR:\tFailed to open I2C bus.\n");
    }
}

int i2c::readBus(uint8_t address, int length, uint8_t *buffer){
    /*
        readBus(uint8_t address, int length, uint8_t buffer[n]) - This method is used to read from a client on the I2C bus.
            Parameters:     
                address     -   7bit address of the client to read data from
                length      -   Length of the transmission in bytes
                buffer      -   Pointer to the memory address to write readings to

            Return:
                0           -   The transmission was successful
                ~0          -   An error occured
    */
   //Contact the client
    if(ioctl(file_i2c, I2C_SLAVE, address) < 0){
        printf("ERROR:\t Failed to contact client");
        return 2;
    }

    //Read data from the client
    if(read(file_i2c, buffer, length) != length){
        printf("ERROR:\tFailed to read bytes from client\n");
        return 3;
    }
    return 0;
}

int i2c::writeBus(uint8_t address, int length, uint8_t writeBuffer[3]){
    /*
    writeBus(uint8_t address, int length, uint8_t buffer[n]) - This method is to write to a client on the I2C bus
            Parameters:
                address     -   7bit address of the client to read data from
                length      -   Length of the transmission in bytes
                buffer      -   array containing the bytes to be written to the bus. Index 0 of the array will be the first byte transmitted

            Return:
                0           -   The transmission was successful
                ~0          -   An error occured
    */

    //Contact the client on the I2C bus
    if(ioctl(file_i2c, I2C_SLAVE, address) < 0){
        printf("ERROR:\t Failed to contact client");
        return 2;
    }

    //Write the bytes from the buffer to the device on the bus
    if (write(file_i2c, writeBuffer, length) != length)
	{
		printf("ERROR:\tFailed to write to slave.\n");
        return 4;
	}
    
    return 0;
}

void i2c::closeBus(){
    /*closeBus() - This method is to close the I2C bus
    */
    close(file_i2c);
}
