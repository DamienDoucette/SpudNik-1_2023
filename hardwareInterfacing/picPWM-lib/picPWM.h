#ifndef PICPWM_H
#define PICPWM_H

#include <stdint.h>
#include "../i2c-lib/i2c.h"

class picPWM {
    private:
        uint8_t addr;
        i2c& I2C;

    public:
        picPWM(uint8_t address, i2c& I2Cobj);

        int setMagX(uint8_t duty_cycle);
        int setMagY(uint8_t duty_cycle);
        int setMagZ(uint8_t duty_cycle);

        int setMotX(uint8_t duty_cycle);
        int setMotY(uint8_t duty_cycle);
};

#endif