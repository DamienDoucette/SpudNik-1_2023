#include "motorDriver.h"

motorDriver::motorDriver(int pwm, int ena, int dir){
    /*Define pin values*/
    pwmPin = pwm;
    enaPin = ena;
    dirPin = dir;

    enable = 1;     //Enable motor by default, can be disabled later with Obj.enable = 0;

    wiringPiSetup();
    pinMode(pwmPin, PWM_OUTPUT);    //Set pinmode of PWM pin
    pinMode(enaPin, OUTPUT);        //Set pinmode of ena pin
    pinMode(dirPin, OUTPUT);        //Set pinmode of dir pin

    digitalWrite(enaPin, enable);   //Enables the motor
}

motorDriver::rotate(int pwm, bool dir){
    digitalWrite(dirPin, dir);  //Set the direction of the motor -- 0 is CW, 1 is CCW

    pwmWrite(pwmPin, pwm);  //Set the speed -- PWM ranges from 0-1024 for 0-100% duty
}