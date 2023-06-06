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

void setup(){

}

void loop(){
    sensingAndEstimation();
}

int main()
{
    setup();

    while(1){
        loop();
    }
}