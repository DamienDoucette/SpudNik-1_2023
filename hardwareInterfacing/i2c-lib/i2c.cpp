/**
 * @file i2c.cpp
 * @author Damien Doucette (dldoucette@upei.ca)
 * @brief C++ file for library to use the I2C on the Raspberry PI - Provides simpler interface for linux/i2c-dev.h
 * @version 0.1
 * @date 2023-07-14
 * 
 * @copyright Copyright (c) 2023
 * 
 */

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

int i2c::writeReadBus(uint8_t address, int length, uint8_t *command, bool *R_W){
    /*
    writeReadBus(uint8_t address, int length, uint8_t *command, bool *R_W) - This method is to write to a client on the I2C bus
            Parameters:
                address     -   7bit address of the client to read data from
                length      -   Number of commands
                command     -   Pointer to array holding the write commands and acting as a buffer to store the read commands
                R_W         -   Pointer to an array holding a 1 (read) or 0 (write) to determine transaction

            Return:
                0           -   The transmission was successful
                ~0          -   An error occured
    */

    struct i2c_msg msg[length];     //Initialize structure for messages

    /*Format each of the messages in the msg structure*/
    for(int i = 0; i < length; i++){
        msg[i].addr = address;  //Set the client address
        msg[i].flags = R_W[i];  //Set read (1) or write (0)
        msg[i].len = 1;         //Set length of message
        msg[i].buf = command;   //Provide the read/write buffer
        command++;              //Increment the pointer to the buffer
    }

    struct i2c_rdwr_ioctl_data ioctl_data = {msg, length};  //Format the messages into the read/write structure

    /*Perform transaction*/
    if(ioctl(file_i2c, I2C_RDWR, &ioctl_data) != length){
        printf("ERROR:\tFailed to complete read/write command");
        return 1;
    }
    return 0;
}

void i2c::closeBus(){
    /*closeBus() - This method is to close the I2C bus
    */
    close(file_i2c);
}
