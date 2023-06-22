#ifndef MOTORDRIVER_H
#define MOTORDRIVER_H

#include <lgpio.h>

class motorDriver {
    private:
        int pwmPin;
        int enaPin;
        int dirPin;

    public:
        bool enable;
        motorDriver(int pwm, int ena, int dir);

        void rotate(float pwm, bool dir);

        void close();

}

#endif