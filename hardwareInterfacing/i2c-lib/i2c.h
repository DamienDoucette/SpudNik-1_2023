#ifndef I2C_H
#define I2C_H

/*I2C libraries*/
#include <unistd.h>				//Needed for I2C port
#include <fcntl.h>				//Needed for I2C port
#include <sys/ioctl.h>			//Needed for I2C port
#include <linux/i2c-dev.h>		//Needed for I2C port

/*Other libraries*/
#include <cstdint>
#include <stdio.h>

class i2c {
    /*  This class is to provide a simple I2C interface to allow data to be 
        written to and read from the bus.

        The constructor takes a single argument with is the I2C bus file location
        This is set to a default value of "/dev/i2c-1" which should be correct for the RPI

        The class has three methods:

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
                buffer      -   array containing the bytes to be written to the bus. Index 0 of the array will be the first byte transmitted

            Return:
                0           -   The transmission was successful
                ~0          -   An error occured

        closeBus() - This method is to close the I2C bus
     */
    private:
        /* Declare private variables */
        int file_i2c;       //Variable to read and write to the I2C file
        char *filename;     //Pointer to the I2C bus file

    public:
    /* Delcare methods */
        i2c(const char* i2cBus = "/dev/i2c-1");
        int readBus(uint8_t address, int length, uint8_t *buffer);
        int writeBus(uint8_t address, int length, uint8_t *buffer);
        void closeBus();

};

#endif
