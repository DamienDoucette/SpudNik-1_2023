#include "i2c.h"

i2c::i2c(int address, char i2cBus){
    int addr = address;
    char *filename = (char*)i2cBus;

    int file_i2c;

    /*Open I2C Bus*/
    if((file_i2c = open(filename, O_RDWR)) < 0){
        printf("ERROR:\tFailed to open I2C bus.\n");
        return 1;
    } else {
        if(ioctl(file_i2c, I2C_SLAVE, addr) < 0){
        printf("ERROR:\tFailed to contact slave at address %d\n", addr);
        return 2;
        }
    }
    return 0;
}

i2c::read(int length){
    /*Read data from IMU*/
    if(read(file_i2c, buffer, length) != length){
        printf("ERROR:\tFailed to read bytes from slave\n");
        return 1;
    } else {
        return buffer;
    }
}

i2c::write(int length, char buffer){
    if (write(file_i2c, buffer, length) != length)
	{
		printf("ERROR:\tFailed to write to slave.\n");
        return 1;
	}
    return 0;
}