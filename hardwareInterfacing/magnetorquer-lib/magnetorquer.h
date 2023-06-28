#ifndef MAGNETORQUER_H
#define MAGNETORQUER_H

#include <lgpio.h>

class magnetorquer{
    private:
        int pwmPin;
        int dirPin;
        float r;

    public:
        magnetorquer(int pwm, int dir, float resistance);

        void setPWM(float current, bool dir);

        void close();

}
#endif