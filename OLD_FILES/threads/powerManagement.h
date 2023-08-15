#ifndef POWERMANAGEMENT_H
#define POWERMANAGEMENT_H

#include <stdio.h>  

//Define Flag variables that are not defined in Power Management thread
extern bool startFlag;
extern bool pwrFlag;
extern bool comFlag;
extern bool imgFlag;

//Define functions required under the Power Management thread
/*NOTE: Output types will need to be fixed at a later time - output type is not known at this time*/

float checkBattery();

#endif