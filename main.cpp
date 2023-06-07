/**
 * @file main.cpp
 * @author Damien Doucette (dldoucette@upei.ca)
 * @brief Main control software for the UPEI Spudnik-1 CubeSat
 * @version 0.1
 * @date 2023-06-06
 * 
 * @copyright Copyright (c) 2023
 * 
 */

/*Include external files*/

//C++ built in libraries
#include <iostream>
#include <thread>

//Local files for various threads functions
#include "threads/acs.h"
#include "threads/powerManagement.h"

//SGP4 library
#include "sgp4/SGP4.h"

using namespace std;

//Declare global variables
bool startFlag = 1;
bool pwrFlag = 0;
bool comFlag = 0;
bool imgFlag = 0;
char TLE[2][69];


void acs(){
    /*
    This function will contain the thread for the ACS software for the Satellite

    Sub-functions for this thread are stored in a seperate file under the 'threads' folder

    The logic for this thread was based off the '0.Main.drawio' file in the 'ConOps' GDrive
    */

    //Delcare local vairables
    double w = 0;
    double posErr = 0;
    double bField[3];
    double w_RWA[2];
    uint8_t ATT_MOD = 0x00; //Will have to check what defualt value is supposed to be

    //Logic defined in FlowChart
    printf("LOG:\tACS Function started\n");
    if(startFlag){
        //sensingAndEstimation();
        if(pwrFlag){
            if(w < 1){
                if(posErr < 1){
                    if(comFlag){
                        ATT_MOD = 0x10;
                        readTargetAttitudeCOMMS();

                        positionControl();

                        if(1){ //Target attitude met
                            //Set attFlag
                        } else {
                            //Reset attFlag
                        }
                    } else {
                        if(imgFlag){
                            ATT_MOD = 0x11;
                            readTargetAttitudeIMG();

                            positionControl();

                            if(1){ //Target attitude met
                            //Set attFlag
                            } else {
                                //Reset attFlag
                            }
                        } else {
                            ATT_MOD = 0x00;
                            detumble();
                        }
                    }
                } else {
                    ATT_MOD = 0x01;
                    calibration();
                }

            } else {
                ATT_MOD = 0x00;
                detumble(); //Might need arguements? one call uses them and one doesnt in flowchart
            }
        }
    }
}

void supervisory(){

}


void powerManagement(){
    /*  This function is used to moniter the power of the satellite

        The function will interface with the battery charge module to determine the battery level
        and will control what power mode the CubeSat is in depending on the battery level

    */
   printf("LOG:\tPower Management Function started\n");

   //Delcare local variables
    float battThresh = 1;   //Define battery level threshold
    float battLevel = checkBattery();

    if(battLevel < battThresh){
        pwrFlag = 1;
    } else {
        pwrFlag = 0;
    }

}


void payloadCOMMS(){

}


void setup(){
    printf("LOG:\tBeginning setup loop...\n");
    
    printf("LOG:\tSetup Complete.\n");

}

void loop(){
    
}

int main()
{
    setup();
    thread acsThread(acs);  //Start ACS thread
    printf("LOG:\tACS Thread called...\n");

    thread powerManagementThread(powerManagement);
    printf("LOG:\tPower Management Thread called...\n");

    acsThread.join();   //When ACS threah finishes, join back to the main thread
    printf("LOG:\tACS Thread ended.\n");

    powerManagementThread.join();
    printf("LOG:\tPower Management Thread ended./n");

    // while(1){
    //     loop();
    // }

    printf("LOG:\t main.cpp completed.  Script will terminate.\n");

}