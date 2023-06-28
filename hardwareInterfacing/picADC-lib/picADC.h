#ifndef PICADC_H
#define PICADC_H

#include <stdint.h>
#include "../i2c-lib/i2c.h"

class picADC{
    private:
        uint8_t addr;
        i2c& I2C;

    public:
        picADC(uint8_t address, i2c& I2Cobj);

        uint16_t* readSunSensors(uint8_t address);
}
#endif