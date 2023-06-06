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

//Include External libraries
#include <iostream>
#include <thread>

//Include Local libraries
#include "threads/acs.h"

using namespace std;

//Declare global variables
bool startFlag = 1;
bool pwrFlag = 0;
bool comFlag = 0;
bool imgFlag = 0;

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
    printf("ACS Function started\n");
    if(startFlag){
        printf("Start flag true\n");
        sensingAndEstimation();
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

void setup(){
    printf("Beginning setup loop...\n");

    printf("Setup Complete.\n");

}

void loop(){
    
}

int main()
{
    setup();

    thread acsThread(acs);
    printf("ACS Thread started...\n");

    acsThread.join();

    // while(1){
    //     loop();
    // }

}