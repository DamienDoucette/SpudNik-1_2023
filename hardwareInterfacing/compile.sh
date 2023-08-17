#! /bin/bash

#This is a script to compile the 'hardwareTesting.cpp file. It runs the compiler and passes all the other files needed
#User may need to give this execute privilages -- In terminal, run 'sudo chmod +x compile.sh'
#Once that is done, you should be able to write './compile.sh' and this script will run, compiling the code
#This will create a new file called 'a.out'
#Use the command 'sudo ./a/out' in order to run the compiled code

sudo g++ hardwareTesting.cpp i2c-lib/i2c.cpp picPWM-lib/picPWM.cpp picADC-lib/picADC.cpp imu-lib/imu.cpp