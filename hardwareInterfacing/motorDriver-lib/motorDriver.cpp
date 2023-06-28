#include "motorDriver.h"

motorDriver::motorDriver(int pwm, int ena, int dir){
    /*Define pin values*/
    pwmPin = pwm;
    enaPin = ena;
    dirPin = dir;
    h = lgGpiochipOpen(0);

    enable = 1;     //Enable motor by default, can be disabled later with Obj.enable = 0;

    lgGpioWrite(h, enaPin, enable); //Enables the motor

}

motorDriver::rotate(float pwm, bool dir){
    float freq = 5000.0;
    lgGpioWrite(h, dirPin, dir);  //Set the direction of the motor -- 0 is CW, 1 is CCW

    lgTxPwm(h, pwmPin, freq, pwm, 0, 0);  //Set the speed -- PWM ranges from 0-100 (float) for 0-100% duty
}

motorDriver::close(){
    lgGpiochipClose(h);
}