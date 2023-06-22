#include "magnetorquer.h"

magnetorquer::magnetorquer(int pwm, int dir, float resistance){
    /*Initialize pin variables and the coils resistance*/
    pwmPin = pwm;
    dirPin = dir;
    r = resistance;

    h = lgGpiochipOpen(0);  //Open GPIO chip

    //Initialize pins to low (PWM will not work without initializing)
    lgGpioWrite(h, pwmPin, 0);
    lgGpioWrite(h, dirPin, 0);
}

magnetorquer::setPWM(float current, bool dir){
    float freq = 5000.0;
    float V = 5.0   //Supply voltage to H-bridge

    Imax = V/r;     //Determine max current
    float pwm = 100.0/Imax * current;   //Convert current to PWM signal

    //Set direction and PWM to pins
    lgGpioWrite(h, dirPin, dir);
    lgTxPwm(h, pin, freq, pwm, 0.0, 0);
}

magnetorquer::close(){
    lgGpiochipClose(h);
}