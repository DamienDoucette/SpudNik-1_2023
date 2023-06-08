#ifndef I2C_H
#define I2C_H

/*I2C libraries*/
#include <unistd.h>				//Needed for I2C port
#include <fcntl.h>				//Needed for I2C port
#include <sys/ioctl.h>			//Needed for I2C port
#include <linux/i2c-dev.h>		//Needed for I2C port

#include <stdio.h>

class i2c {
    private:
        int addr;
        char *filename;
        unsigned char buffer[64];

    public:
        int i2c(int address, char i2cBus);
        int read(int length);
        int write(int length);

}

#endif