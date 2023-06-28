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
#include <cstring>

class i2c {
    private:
        int addr;
        int file_i2c;
        char *filename;
        uint8_t buffer[60];

    public:
        i2c(const char* i2cBus);
        uint8_t *readBus(uint8_t address, int length);
        int writeBus(uint8_t address, int length, uint8_t buffer[3]);
        void closeBus();

};

#endif
