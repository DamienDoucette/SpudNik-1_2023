#ifndef ACS_H
#define ACS_H

#include <stdio.h>  

//Define Flag variables that are not defined in ACS thread
extern bool startFlag;
extern bool pwrFlag;
extern bool comFlag;
extern bool imgFlag;

//Define functions required under the ACS thread
/*NOTE: Output types will need to be fixed at a later time - output type is not known at this time*/

void sensingAndEstimation();
void detumble();
void calibration();
void readTargetAttitude();
void positionControl();

#endif