#include "i2c.h"

i2c::i2c(const char* i2cBus){
    char *filename = (char*)i2cBus;

    /*Open I2C Bus*/
    if((file_i2c = open(filename, O_RDWR)) < 0){
        printf("ERROR:\tFailed to open I2C bus.\n");
    }
}

uint8_t* i2c::readBus(uint8_t address, int length){
    if(ioctl(file_i2c, I2C_SLAVE, address) < 0){
        printf("ERROR:\t Failed to contact client");
    }
    if(read(file_i2c, buffer, length) != length){
        printf("ERROR:\tFailed to read bytes from client\n");
    } else {
        
    }
    return buffer;
}

int i2c::writeBus(uint8_t address, int length, uint8_t writeBuffer[3]){

    if(ioctl(file_i2c, I2C_SLAVE, address) < 0){
        printf("ERROR:\t Failed to contact client");
        return 2;
    }

    if (write(file_i2c, writeBuffer, length) != length)
	{
		printf("ERROR:\tFailed to write to slave.\n");
        return 4;
	}
    return 0;
}

void i2c::closeBus(){
    close(file_i2c);
}
