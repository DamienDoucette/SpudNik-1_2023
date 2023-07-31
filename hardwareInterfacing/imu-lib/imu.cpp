/**
 * @file imu.cpp
 * @author Damien Doucette (dldoucette@upei.ca)
 * @brief C++ file for library to interface ICM-20948 IMU with the Raspberry PI over I2C
 * @version 0.1
 * @date 2023-07-14
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "imu.h"

imu::imu(i2c& I2Cobj, uint8_t address){
    /*Class Constructor:
        imu(i2c& I2Cobj, uint8_t address)
        
        Parameters:
            I2Cobj  -   Reference to an object of the I2C class
            address -   7bit client address of the IMU -- Default value will be assigned if none are provided
    */

    addr = address;     //Assign client address to the global variable
    I2C = I2Cobj;       //Assign the I2C reference to the to the I2C object

    /*Run the CONFIG update functions to send the default configuration to the IMU*/
    update_USER_CTRL();
    update_PWR_MGMT();
    update_ACCEL_CONFIG();
    update_GYRO_CONFIG();
    update_MAG_CONFIG();
}

void imu::update_USER_CTRL(){
    /*update_USER_CTRL()    -   This is a method to format the configuration options stored in the public USER_CTRL structure of the imu instance
        This data is then written to the IMU to update the USER_CTRL register
        configuration options can be adjusted prior to this command by simple writting to the structure. EX: imu.USER_CTRL.DMP_EN = 1;
    */

    changeBank(0x00);   //Change to register bank 0
    uint8_t REG_USER_CTRL = 0x03;   //Address of the USER_CTRL register

    uint8_t USER_CTRL_byte = 0x00;  //Initize an empty byte to hold the configuration parameters

    /*Use the global structure variables for the USER_CTRL and format them
    into the order that is outlined in the datasheet for the IMU*/
    USER_CTRL_byte |= USER_CTRL.DMP_EN << 7;
    USER_CTRL_byte |= USER_CTRL.FIFO_EN << 6;
    USER_CTRL_byte |= USER_CTRL.I2C_MST_EN << 5;
    USER_CTRL_byte |= USER_CTRL.I2C_IF_DIS << 4;
    USER_CTRL_byte |= USER_CTRL.DMP_RST << 3;
    USER_CTRL_byte |= USER_CTRL.SRAM_RST << 2;
    USER_CTRL_byte |= USER_CTRL.I2C_MST_RST << 1;

    //Setup the write buffer for the I2C transaction
    uint8_t command[2] = {REG_USER_CTRL, USER_CTRL_byte};

    //printf("USER_CTRL:\t%d\n", command[1]);   //Uncomment to see the formatted byte being sent

    I2C.writeBus(addr, 2, command); //Send the register address followed by the value to the IMU to update the USER_CTRL register
}

void imu::update_PWR_MGMT(){
    /*update_PWR_MGMT()    -   This is a method to format the configuration options stored in the public PWR_MGMT structure of the imu instance
        This data is then written to the IMU to update the PWR_MGMT_1 and PWR_MGMT_2 registers
        configuration options can be adjusted prior to this command by simple writting to the structure. EX: imu.PWR_MGMT.SLEEP = 1;
    */

    changeBank(0x00);   //Change to register bank 0
    uint8_t command[2]; //Initialize the write buffer variable

    uint8_t REG_PWR_MGMT_1 = 0x06;  //Save the address for the PWR_MGMT_1 register on the IMU
    uint8_t REG_PWR_MGMT_2 = 0x07;  //Save the address for the PWr_MGMT_2 register on the IMU

    uint8_t PWR_MGMT_1 = 0x00;      //Initialize the PWR_MGMT_1 byte to 0

    /*Format the various configuration parameters for PWR_MGMT_1 into the format outlined in the datasheet*/
    PWR_MGMT_1 |= PWR_MGMT.DEVICE_RESET << 7;
    PWR_MGMT_1 |= PWR_MGMT.SLEEP << 6;
    PWR_MGMT_1 |= PWR_MGMT.LP_EN << 5;
    PWR_MGMT_1 |= PWR_MGMT.TEMP_DIS << 3;
    PWR_MGMT_1 |= (PWR_MGMT.CLKSEL & 0x03);

    uint8_t PWR_MGMT_2 = 0x00;  //Initialize the PWR_MGMT_2 byte to 0
    /*Set/clear the bits for disabling the accelerometer and gyroscope depending on the PWR_MGMT.DISABLE_ACCEL and PWR_MGMT.DISABLE_GYRO bits*/
    if(PWR_MGMT.DISABLE_ACCEL){
        PWR_MGMT_2 |= 0b00111000;   //Set the DISABLE_ACCEL bits of the PWR_MGMT_2 byte
    } else {
        PWR_MGMT_2 &= ~(0b00111000);    //Clear the DISABLE_ACCEL bits of the PWR_MGMT_2 byte
    }
    if(PWR_MGMT.DISABLE_GYRO){
        PWR_MGMT_2 |= 0b00000111;   //Set the DISABLE_GYRO bits of the PWR_MGMT_2 byte
    } else {
        PWR_MGMT_2 &= ~(0b00000111);    //Clear the DISABLE_GYRO bits of the PWR_MGMT_2 byte
    }

    //Format the write buffer for updating the PWR_MGMT_1 register
    command[0] = REG_PWR_MGMT_1;
    command[1] = PWR_MGMT_1;
    //printf("PWR_MGMT_1:\t%d\n", command[1]);  //Uncomment to see the formatted byte being sent
    I2C.writeBus(addr, 2, command); //Write the configuration byte to the PWR_MGMT_1 register on the IMU

    //Format the write buffer for updating the PWR_MGMT_2 register
    command[0] = REG_PWR_MGMT_2;
    command[1] = PWR_MGMT_2;
    //printf("PWR_MGMT_2:\t%d\n", command[1]);  //Uncomment to see the formatted byte being sent
    I2C.writeBus(addr, 2, command); //Write the configuration byte to the PWR_MGMT_2 register on the IMU
}

void imu::update_ACCEL_CONFIG(){
    /*update_ACCEL_CONFIG()    -   This is a method to format the configuration options stored in the public ACCEL_CONFIG structure of the imu instance
        This data is then written to the IMU to update the ACCEL_SMPLRT_DIV_1, ACCEL_SMPLRT_DIV_2, ACCEL_INTEL_CTRL, ACCEL_CONFIG_1, ACCEL_CONFIG_2 registers
        configuration options can be adjusted prior to this command by simple writting to the structure. EX: imu.ACCEL_CONFIG.ACCEL_FCHOICE = 1;
    */

    changeBank(0x02);   //Change to register bank 2
    uint8_t command[2]; //Declare variable for write buffer

    /*Store the addresses of the variable Accelerometer configuration registers*/
    uint8_t REG_ACCEL_SMPLRT_DIV_1 = 0x10;
    uint8_t REG_ACCEL_SMPLRT_DIV_2 = 0x11;
    uint8_t REG_ACCEL_INTEL_CTRL = 0x12;
    uint8_t REG_ACCEL_CONFIG_1 = 0x14;
    uint8_t REG_ACCEL_CONFIG_2 = 0x15;

    /*Initialize the configuration bytes for each register to 0*/
    uint8_t ACCEL_SMPLRT_DIV_1 = 0x00;
    uint8_t ACCEL_SMPLRT_DIV_2 = 0x00;
    uint8_t ACCEL_INTEL_CTRL = 0x00;
    uint8_t ACCEL_CONFIG_1 = 0x00;
    uint8_t ACCEL_CONFIG_2 = 0x00;

    /*Format each configuration byte to the data format outlined in the datasheet*/
    ACCEL_SMPLRT_DIV_1 = ACCEL_CONFIG.ACCEL_SMPLRT_DIV >> 8;    //Set the ACCEL_SMPLRT_DIV_1 (MSB) byte

    ACCEL_SMPLRT_DIV_2 = ACCEL_CONFIG.ACCEL_SMPLRT_DIV & 0xFF;  //Set the ACCEL_SMPLRT_DIV_2 (LSB) byte

    ACCEL_INTEL_CTRL |= ACCEL_CONFIG.ACCEL_INTEL_EN << 1;   //Set the ACCEL_INTEL_EN bit of the ACCEL_INTEL_CTRL byte 
    ACCEL_INTEL_CTRL |= ACCEL_CONFIG.ACCEL_INTEL_MODE_INT;  //Set the ACCEL_INTEL_MODE_INT bit of the ACCEL_INTEL_CTRL byte 

    ACCEL_CONFIG_1 |= (ACCEL_CONFIG.ACCEL_DLPFCFG & 0x07) << 3; //Set the ACCEL_DLPFCFG bits of the ACCEL_CONFIG_1 byte
    ACCEL_CONFIG_1 |= (ACCEL_CONFIG.ACCEL_FS_SEL & 0x03) << 1;  //Set the ACCEL_FS_SEL bits of the ACCEL_CONFIG_1 byte
    ACCEL_CONFIG_1 |= ACCEL_CONFIG.ACCEL_FCHOICE;               //Set the ACCEL_FCHOICE bit of the ACCEL_CONFIG_1 byte

    ACCEL_CONFIG_2 |= ACCEL_CONFIG.AX_ST_EN_REG << 4;   //Set the AX_ST_EN_REG bit of the ACCEL_CONFIG_2 byte
    ACCEL_CONFIG_2 |= ACCEL_CONFIG.AY_ST_EN_REG << 3;   //Set the AY_ST_EN_REG bit of the ACCEL_CONFIG_2 byte
    ACCEL_CONFIG_2 |= ACCEL_CONFIG.AZ_ST_EN_REG << 2;   //Set the AZ_ST_EN_REG bit of the ACCEL_CONFIG_2 byte
    ACCEL_CONFIG_2 |= ACCEL_CONFIG.DEC3_CFG & 0x03;     //Set the DEC3_CFG bits of the ACCEL_CONFIG_2 byte
    
    //Format the write buffer for updating the ACCEL_SMPLRT_DIV_1 register
    command[0] = REG_ACCEL_SMPLRT_DIV_1;
    command[1] = ACCEL_SMPLRT_DIV_1;
    //printf("ACCEL_SMPLRT_DIV_1:\t%d\n", command[1]);  //Uncomment to see the formatted byte being sent
    I2C.writeBus(addr, 2, command);     //Write the byte to the IMU register

    //Format the write buffer for updating the ACCEL_SMPLRT_DIV_2 register
    command[0] = REG_ACCEL_SMPLRT_DIV_2;
    command[1] = ACCEL_SMPLRT_DIV_2;
    //printf("ACCEL_SMPLRT_DIV_2:\t%d\n", command[1]);  //Uncomment to see the formatted byte being sent
    I2C.writeBus(addr, 2, command);     //Write the byte to the IMU register

    //Format the write buffer for updating the ACCEL_INTEL_CTRL register
    command[0] = REG_ACCEL_INTEL_CTRL;
    command[1] = ACCEL_INTEL_CTRL;
    //printf("ACCEL_INTEL_CTRL:\t%d\n", command[1]);    //Uncomment to see the formatted byte being sent
    I2C.writeBus(addr, 2, command);     //Write the byte to the IMU register

    //Format the write buffer for updating the ACCEL_CONFIG_1 register
    command[0] = REG_ACCEL_CONFIG_1;
    command[1] = ACCEL_CONFIG_1;
    //printf("ACCEL_CONFIG_1:\t%d\n", command[1]);      //Uncomment to see the formatted byte being sent
    I2C.writeBus(addr, 2, command);     //Write the byte to the IMU register

    //Format the write buffer for updating the ACCEL_CONFIG_2 register
    command[0] = REG_ACCEL_CONFIG_2;
    command[1] = ACCEL_CONFIG_2;
    //printf("ACCEL_CONFIG_2:\t%d\n", command[1]);      //Uncomment to see the formatted byte being sent
    I2C.writeBus(addr, 2, command);     //Write the byte to the IMU register
}

void imu::update_GYRO_CONFIG(){
    /*update_GYRO_CONFIG()    -   This is a method to format the configuration options stored in the public GYRO_CONFIG structure of the imu instance
        This data is then written to the IMU to update the GYRO_SMPLRT_DIV, GYRO_CONFIG_1, GYRO_CONFIG_2 registers
        configuration options can be adjusted prior to this command by simple writting to the structure. EX: imu.GYRO_CONFIG.GYRO_FS_SEL = 1;
    */

    changeBank(0x02);   //Change to register bank 2
    uint8_t command[2]; //Delare variable for the write buffer

    /*Save the addresses of the gyroscope config registers*/
    uint8_t REG_GYRO_SMPLRT_DIV = 0x00;
    uint8_t REG_GYRO_CONFIG_1 = 0x01;
    uint8_t REG_GYRO_CONFIG_2 = 0x02;

    /*Initialize the config bytes to 0*/
    uint8_t GYRO_SMPLRT_DIV = 0x00;
    uint8_t GYRO_CONFIG_1 = 0x00;
    uint8_t GYRO_CONFIG_2 = 0x00;

    GYRO_SMPLRT_DIV = GYRO_CONFIG.GYRO_SMPLRT_DIV;  //Set the GYRO_SMPLRT_DIV register

    GYRO_CONFIG_1 |= (GYRO_CONFIG.GYRO_DLPFCFG & 0x07) << 3;    //Set the GYRO_DLPFCFG bits of the GYRO_CONFIG_1 byte
    GYRO_CONFIG_1 |= (GYRO_CONFIG.GYRO_FS_SEL & 0x03) << 1;     //Set the GYRO_FS_SEL bits of the GYRO_CONFIG_1 byte
    GYRO_CONFIG_1 |= GYRO_CONFIG.GYRO_FCHOICE;                  //Set the GYRO_FCHOICE bit of the GYRO_CONFIG_1 byte

    GYRO_CONFIG_2 |= GYRO_CONFIG.XGYRO_CTEN << 5;   //Set the XGYRO_CTEN bit of the GYRO_CONFIG_2 byte
    GYRO_CONFIG_2 |= GYRO_CONFIG.YGYRO_CTEN << 4;   //Set the YGYRO_CTEN bit of the GYRO_CONFIG_2 byte
    GYRO_CONFIG_2 |= GYRO_CONFIG.ZGYRO_CTEN << 3;   //Set the ZGYRO_CTEN bit of the GYRO_CONFIG_2 byte
    GYRO_CONFIG_2 |= GYRO_CONFIG.GYRO_AVGCFG & 0x07;    //Set the GYRO_AVGCFG bits of the GYRO_CONFIG_2 byte

    //Format the write buffer to update the GYRO_SMPLRT_DIV register
    command[0] = REG_GYRO_SMPLRT_DIV;
    command[1] = GYRO_SMPLRT_DIV;
    //printf("GYRO_SMPLRT_DIV:\t%d\n", command[1]); //Uncomment to see the formatted byte being sent
    I2C.writeBus(addr, 2, command); //Write the configuration byte to the register on the IMU

    //Format the write buffer to update the GYRO_CONFIG_1 register
    command[0] = REG_GYRO_CONFIG_1;
    command[1] = GYRO_CONFIG_1;
    //printf("GYRO_CONFIG_1:\t%d\n", command[1]);   //Uncomment to see the formatted byte being sent
    I2C.writeBus(addr, 2, command); //Write the configuration byte to the register on the IMU

    //Format the write buffer to update the GYRO_CONFIG_2 register
    command[0] = REG_GYRO_CONFIG_2;
    command[1] = GYRO_CONFIG_2;
    //printf("GYRO_CONFIG_2:\t%d\n", command[1]);   //Uncomment to see the formatted byte being sent
    I2C.writeBus(addr, 2, command); //Write the configuration byte to the register on the IMU
}

void imu::update_MAG_CONFIG(){
    /*update_MAG_CONFIG()    -   This is a method to format the configuration options stored in the public MAG_CONFIG structure of the imu instance
        This data is then written to the IMU to update the CNTL2, CNTL3 registers
        configuration options can be adjusted prior to this command by simple writting to the structure. EX: imu.MAG_CONFIG.SRST = 1;
    */
    uint8_t magAddr = 0x0C; //Store the address of the magnetometer -- Note this is external to the IMU-20948 chip and shows up as a seperate device on the I2C bus
    uint8_t command[2]; //Delcare an array for the write buffer

    /*Store the addresses of the control registers*/
    uint8_t REG_CNTL2 = 0x31;
    uint8_t REG_CNTL3 = 0x32;

    uint8_t CNTL2 = MAG_CONFIG.MODE & 0x1F; //Set MODE bits to the CNTL2 byte
    uint8_t CNTL3 = MAG_CONFIG.SRST;        //Set the SRST bit to the CNTL3 byte

    //Format the write buffer to update the CNTL2 register
    command[0] = REG_CNTL2;
    command[1] = CNTL2;
    //printf("CNTL2:\t%d\n", command[1]);   //Uncomment to see the formatted byte being sent
    I2C.writeBus(magAddr, 2, command);  //Write the configuration byte to the register on the IMU
    
    //Format the write buffer to update the CNTL3 register
    command[0] = REG_CNTL3;
    command[1] = CNTL3;
    //printf("CNTL3:\t%d\n", command[1]);   //Uncomment to see the formatted byte being sent
    I2C.writeBus(magAddr, 2, command);  //Write the configuration byte to the register on the IMU
}

void imu::setGyroOffset(int16_t x, int16_t y, int16_t z){
    /*setGyroOffset(int16_t x, int16_t y, int16_t z)    -   This is a method used to set the offset registers for the gyroscope
        Parameters:
            x   -   16bit value to define the offset of the gyroscope in x
            y   -   16bit value to define the offset of the gyroscope in y
            z   -   16bit value to define the offset of the gyroscope in z
    */
    changeBank(0x02);   //Change to register bank 2
    uint8_t command[2]; //Delcare a variable for the write buffer

    /*Define the register addresses of the offset registers*/
    uint8_t REG_XG_OFFS_USRH = 0x03;
    uint8_t REG_XG_OFFS_USRL = 0x04;
    uint8_t REG_YG_OFFS_USRH = 0x05;
    uint8_t REG_YG_OFFS_USRL = 0x06;
    uint8_t REG_ZG_OFFS_USRH = 0x07;
    uint8_t REG_ZG_OFFS_USRL = 0x08;

    /*Assign the values form the parameters to a byte to be written to each register
    Note: The registers are split into an 8bit High and 8bit Low register*/
    uint8_t XG_OFFS_USRH = x << 8;
    uint8_t XG_OFFS_USRL = x & 0xFF;
    uint8_t YG_OFFS_USRH = y << 8;
    uint8_t YG_OFFS_USRL = y & 0xFF;
    uint8_t ZG_OFFS_USRH = z << 8;
    uint8_t ZG_OFFS_USRL = z & 0xFF;

    //Format the write buffer to update the XG_OFFS_USERH register
    command[0] = REG_XG_OFFS_USRH;
    command[1] = XG_OFFS_USRH;
    I2C.writeBus(addr, 2, command); //Write the byte to the IMU register

    //Format the write buffer to update the XG_OFFS_USERL register
    command[0] = REG_XG_OFFS_USRL;
    command[1] = XG_OFFS_USRL;
    I2C.writeBus(addr, 2, command); //Write the byte to the IMU register

    //Format the write buffer to update the YG_OFFS_USERH register
    command[0] = REG_YG_OFFS_USRH;
    command[1] = YG_OFFS_USRH;
    I2C.writeBus(addr, 2, command); //Write the byte to the IMU register

    //Format the write buffer to update the YG_OFFS_USERL register
    command[0] = REG_YG_OFFS_USRL;
    command[1] = YG_OFFS_USRL;
    I2C.writeBus(addr, 2, command); //Write the byte to the IMU register

    //Format the write buffer to update the ZG_OFFS_USERH register
    command[0] = REG_ZG_OFFS_USRH;
    command[1] = ZG_OFFS_USRH;
    I2C.writeBus(addr, 2, command); //Write the byte to the IMU register

    //Format the write buffer to update the ZG_OFFS_USERL register
    command[0] = REG_ZG_OFFS_USRL;
    command[1] = ZG_OFFS_USRL;
    I2C.writeBus(addr, 2, command); //Write the byte to the IMU register
}

void imu::setAccelOffset(int16_t x, int16_t y, int16_t z){
    /*setAccelOffset(int16_t x, int16_t y, int16_t z)    -   This is a method used to set the offset registers for the accelerometer
        Parameters:
            x   -   16bit value to define the offset of the accelerometer in x
            y   -   16bit value to define the offset of the accelerometer in y
            z   -   16bit value to define the offset of the accelerometer in z
    */

    changeBank(0x01);   //Change to register bank 2
    uint8_t command[2]; //Delcare a variable for the write buffer

    /*Store the addresses of the accelerometer offset registers*/
    uint8_t REG_XA_OFFS_USRH = 0x14;
    uint8_t REG_XA_OFFS_USRL = 0x15;
    uint8_t REG_YA_OFFS_USRH = 0x17;
    uint8_t REG_YA_OFFS_USRL = 0x18;
    uint8_t REG_ZA_OFFS_USRH = 0x1A;
    uint8_t REG_ZA_OFFS_USRL = 0x1B;

    /*Assign the values from the parameters to a byte for each of the IMU registers
        NOTE: Data is split into an 8bit High register and an 8bit Low register*/
    uint8_t XA_OFFS_USRH = x << 8;
    uint8_t XA_OFFS_USRL = x & 0xFF;
    uint8_t YA_OFFS_USRH = y << 8;
    uint8_t YA_OFFS_USRL = y & 0xFF;
    uint8_t ZA_OFFS_USRH = z << 8;
    uint8_t ZA_OFFS_USRL = z & 0xFF;

    //Format the write buffer to update the XA_OFFS_USERH register
    command[0] = REG_XA_OFFS_USRH;
    command[1] = XA_OFFS_USRH;
    I2C.writeBus(addr, 2, command); //Write the byte to the IMU register

    //Format the write buffer to update the XA_OFFS_USERL register
    command[0] = REG_XA_OFFS_USRL;
    command[1] = XA_OFFS_USRL;
    I2C.writeBus(addr, 2, command); //Write the byte to the IMU register

    //Format the write buffer to update the YA_OFFS_USERH register
    command[0] = REG_YA_OFFS_USRH;
    command[1] = YA_OFFS_USRH;
    I2C.writeBus(addr, 2, command); //Write the byte to the IMU register

    //Format the write buffer to update the YA_OFFS_USERL register
    command[0] = REG_YA_OFFS_USRL;
    command[1] = YA_OFFS_USRL;
    I2C.writeBus(addr, 2, command); //Write the byte to the IMU register

    //Format the write buffer to update the ZA_OFFS_USERH register
    command[0] = REG_ZA_OFFS_USRH;
    command[1] = ZA_OFFS_USRH;
    I2C.writeBus(addr, 2, command); //Write the byte to the IMU register

    //Format the write buffer to update the ZA_OFFS_USERL register
    command[0] = REG_ZA_OFFS_USRL;
    command[1] = ZA_OFFS_USRL;
    I2C.writeBus(addr, 2, command); //Write the byte to the IMU register
}

void imu::readAccel(int16_t *x, int16_t *y, int16_t *z){
    /*readAccel(int16_t *x, int16_t *y, int16_t *z)    -   This is a method used read the three axes of the accelerometer
        Parameters:
            x   -   Pointer to a 16bit memory location where the x reading is to be stored
            y   -   Pointer to a 16bit memory location where the y reading is to be stored
            z   -   Pointer to a 16bit memory location where the z reading is to be stored

        NOTE: When calling this function, it is important to pass a reference (&) to a int16_t variable, not the variable itself
            EX: int16_t x, y, z;
                readAccel(&x, &y, &z);
    */

    changeBank(0x00);   //Change to register bank 0
    uint8_t command[2]; //Delcare variable for the read/write buffer
    bool rw[2]; //Delcare variable to hold whether each byte in the transmission is a read or write

    /*Store the addresses of the accelerometer output registers*/
    uint8_t REG_ACCEL_XOUT_H = 0x2D;
    uint8_t REG_ACCEL_XOUT_L = 0x2E;
    uint8_t REG_ACCEL_YOUT_H = 0x2F;
    uint8_t REG_ACCEL_YOUT_L = 0x30;
    uint8_t REG_ACCEL_ZOUT_H = 0x31;
    uint8_t REG_ACCEL_ZOUT_L = 0x32;

    /*Set the read/write array to be {0, 1} meaning {write, read}. This format is outlined in the datasheet - 
    the IMU expects the register address you want to read from to be written to it, then the next read request 
    the IMU will return the contents of that register
    */
    rw[0] = 0;
    rw[1] = 1;

    command[0] = REG_ACCEL_XOUT_H;  //Set the write data to be the ACCEL_XOUT_H address
    I2C.writeReadBus(addr, 2, command, rw); //Perform the write/read command. The read data will be stored in index 1 of the buffer

    *x = (uint16_t)command[1] << 8; //Write the MSB of the accelerometer x output to the memory location given in the parameters

    command[0] = REG_ACCEL_XOUT_L;  //Set the write data to be the ACCEL_XOUT_L address
    I2C.writeReadBus(addr, 2, command, rw); //Perform the write/read command. The read data will be stored in index 1 of the buffer

    *x |= (uint16_t)command[1]; //Write the LSB of the accelerometer x output to the memory location given in the parameters

    command[0] = REG_ACCEL_YOUT_H;  //Set the write data to be the ACCEL_YOUT_H address
    I2C.writeReadBus(addr, 2, command, rw); //Perform the write/read command. The read data will be stored in index 1 of the buffer

    *y = (uint16_t)command[1] << 8; //Write the MSB of the accelerometer y output to the memory location given in the parameters

    command[0] = REG_ACCEL_YOUT_L;  //Set the write data to be the ACCEL_YOUT_L address
    I2C.writeReadBus(addr, 2, command, rw); //Perform the write/read command. The read data will be stored in index 1 of the buffer

    *y |= (uint16_t)command[1]; //Write the LSB of the accelerometer y output to the memory location given in the parameters

    command[0] = REG_ACCEL_ZOUT_H;  //Set the write data to be the ACCEL_ZOUT_H address
    I2C.writeReadBus(addr, 2, command, rw); //Perform the write/read command. The read data will be stored in index 1 of the buffer

    *z = (uint16_t)command[1] << 8; //Write the MSB of the accelerometer z output to the memory location given in the parameters

    command[0] = REG_ACCEL_ZOUT_L;  //Set the write data to be the ACCEL_ZOUT_L address
    I2C.writeReadBus(addr, 2, command, rw); //Perform the write/read command. The read data will be stored in index 1 of the buffer

    *z |= (uint16_t)command[1]; //Write the LSB of the accelerometer z output to the memory location given in the parameters
}

void imu::readGyro(int16_t *x, int16_t *y, int16_t *z){
    /*readGyro(int16_t *x, int16_t *y, int16_t *z)    -   This is a method used read the three axes of the Gyroscope
        Parameters:
            x   -   Pointer to a 16bit memory location where the x reading is to be stored
            y   -   Pointer to a 16bit memory location where the y reading is to be stored
            z   -   Pointer to a 16bit memory location where the z reading is to be stored

        NOTE: When calling this function, it is important to pass a reference (&) to a int16_t variable, not the variable itself
            EX: int16_t x, y, z;
                readGyro(&x, &y, &z);
    */

    changeBank(0x00);   //Change to register bank 0
    uint8_t command[2]; //Declare variable for the write/read buffer
    bool rw[2];         //Declare variable to hold whether command is read or write

    /*Store the addresses of the gryoscope output registers*/
    uint8_t REG_GYRO_XOUT_H = 0x33;
    uint8_t REG_GYRO_XOUT_L = 0x34;
    uint8_t REG_GYRO_YOUT_H = 0x35;
    uint8_t REG_GYRO_YOUT_L = 0x36;
    uint8_t REG_GYRO_ZOUT_H = 0x37;
    uint8_t REG_GYRO_ZOUT_L = 0x38;

    /*Set the read/write array to be {0, 1} meaning {write, read}. This format is outlined in the datasheet - 
    the IMU expects the register address you want to read from to be written to it, then the next read request 
    the IMU will return the contents of that register
    */
    rw[0] = 0;
    rw[1] = 1;

    command[0] = REG_GYRO_XOUT_H;   //Set the write data to be the GYRO_XOUT_H address
    I2C.writeReadBus(addr, 2, command, rw); //Perform the write/read command. The read data will be stored in index 1 of the buffer

    *x = (uint16_t)command[1] << 8; //Write the MSB of the gyroscope x output to the memory location given in the parameters

    command[0] = REG_GYRO_XOUT_L;   //Set the write data to be the GYRO_XOUT_L address
    I2C.writeReadBus(addr, 2, command, rw); //Perform the write/read command. The read data will be stored in index 1 of the buffer

    *x |= (uint16_t)command[1]; //Write the LSB of the gyroscope x output to the memory location given in the parameters

    command[0] = REG_GYRO_YOUT_H;   //Set the write data to be the GYRO_YOUT_H address
    I2C.writeReadBus(addr, 2, command, rw); //Perform the write/read command. The read data will be stored in index 1 of the buffer

    *y = (uint16_t)command[1] << 8; //Write the MSB of the gyroscope y output to the memory location given in the parameters

    command[0] = REG_GYRO_YOUT_L;   //Set the write data to be the GYRO_YOUT_L address
    I2C.writeReadBus(addr, 2, command, rw); //Perform the write/read command. The read data will be stored in index 1 of the buffer

    *y |= (uint16_t)command[1]; //Write the LSB of the gyroscope y output to the memory location given in the parameters

    command[0] = REG_GYRO_ZOUT_H;   //Set the write data to be the GYRO_ZOUT_H address
    I2C.writeReadBus(addr, 2, command, rw); //Perform the write/read command. The read data will be stored in index 1 of the buffer

    *z = (uint16_t)command[1] << 8; //Write the MSB of the gyroscope z output to the memory location given in the parameters

    command[0] = REG_GYRO_ZOUT_L;   //Set the write data to be the GYRO_ZOUT_L address
    I2C.writeReadBus(addr, 2, command, rw); //Perform the write/read command. The read data will be stored in index 1 of the buffer

    *z |= (uint16_t)command[1]; //Write the LSB of the gyroscope z output to the memory location given in the parameters
}

void imu::readMag(int16_t *x, int16_t *y, int16_t *z){
    /*readMag(int16_t *x, int16_t *y, int16_t *z)    -   This is a method used read the three axes of the magnetometer
        Parameters:
            x   -   Pointer to a 16bit memory location where the x reading is to be stored
            y   -   Pointer to a 16bit memory location where the y reading is to be stored
            z   -   Pointer to a 16bit memory location where the z reading is to be stored

        NOTE: When calling this function, it is important to pass a reference (&) to a int16_t variable, not the variable itself
            EX: int16_t x, y, z;
                readMag(&x, &y, &z);
    */

    uint8_t magAddr = 0x0C; /*Store the I2C address of the Magnetometer
    NOTE: The magnetometer is not contained in the ICM-20948 and appears as its own device on the bus*/

    uint8_t command[2]; //Delcare variable for the read/write buffer
    bool rw[2]; //Declare variable to hold whether command is read or write

    /*Store the addresses of the output registers of the magnetometer*/
    uint8_t REG_MAG_XOUT_H = 0x12;
    uint8_t REG_MAG_XOUT_L = 0x11;
    uint8_t REG_MAG_YOUT_H = 0x14;
    uint8_t REG_MAG_YOUT_L = 0x13;
    uint8_t REG_MAG_ZOUT_H = 0x16;
    uint8_t REG_MAG_ZOUT_L = 0x15;
    uint8_t REG_MAG_ST2 = 0x18;

    /*Set the read/write array to be {0, 1} meaning {write, read}. This format is outlined in the datasheet - 
    the IMU expects the register address you want to read from to be written to it, then the next read request 
    the IMU will return the contents of that register
    */
    rw[0] = 0;
    rw[1] = 1;

    command[0] = REG_MAG_XOUT_H;    //Set the write data to be the MAG_XOUT_H address
    I2C.writeReadBus(magAddr, 2, command, rw);  //Perform the write/read command. The read data will be stored in index 1 of the buffer

    *x = (uint16_t)command[1] << 8; //Write the MSB of the magnetometer x output to the memory location given in the parameters

    command[0] = REG_MAG_XOUT_L;    //Set the write data to be the MAG_XOUT_L address
    I2C.writeReadBus(magAddr, 2, command, rw);  //Perform the write/read command. The read data will be stored in index 1 of the buffer

    *x |= (uint16_t)command[1]; //Write the LSB of the magnetometer x output to the memory location given in the parameters

    command[0] = REG_MAG_YOUT_H;    //Set the write data to be the MAG_YOUT_H address
    I2C.writeReadBus(magAddr, 2, command, rw);  //Perform the write/read command. The read data will be stored in index 1 of the buffer

    *y = (uint16_t)command[1] << 8; //Write the MSB of the magnetometer y output to the memory location given in the parameters

    command[0] = REG_MAG_YOUT_L;    //Set the write data to be the MAG_YOUT_L address
    I2C.writeReadBus(magAddr, 2, command, rw);  //Perform the write/read command. The read data will be stored in index 1 of the buffer

    *y |= (uint16_t)command[1]; //Write the LSB of the magnetometer y output to the memory location given in the parameters

    command[0] = REG_MAG_ZOUT_H;    //Set the write data to be the MAG_ZOUT_H address
    I2C.writeReadBus(magAddr, 2, command, rw);  //Perform the write/read command. The read data will be stored in index 1 of the buffer

    *z = (uint16_t)command[1] << 8; //Write the MSB of the magnetometer z output to the memory location given in the parameters

    command[0] = REG_MAG_ZOUT_L;    //Set the write data to be the MAG_ZOUT_L address
    I2C.writeReadBus(magAddr, 2, command, rw);  //Perform the write/read command. The read data will be stored in index 1 of the buffer

    *z |= (uint16_t)command[1]; //Write the LSB of the magnetometer z output to the memory location given in the parameters

    /*The datasheet specified that every read of the output registers should be followed by a read of the ST2 register
    The value read is not relevant and does not need to be saved*/
    command[0] = REG_MAG_ST2;   //Set the write data to be the MAG_ST2 address
    I2C.writeReadBus(magAddr, 2, command, rw);  //Perform the write/read command. The read data will be stored in index 1 of the buffer
}

void imu::readTemp(int16_t *t){
    /*readTemp(int16_t *t)    -   This is a method used read the three axes of the accelerometer
        Parameters:
            t   -   Pointer to a 16bit memory location where the x reading is to be stored

        NOTE: When calling this function, it is important to pass a reference (&) to a int16_t variable, not the variable itself
            EX: int16_t t;
                readTemp(&t);
    */

    changeBank(0x00);   //Change to register bank 0
    uint8_t command[2]; //Delcare variable for the read/write buffer
    bool rw[2]; //Declare variable to hold whether command is a read or write

    /*Store the addresses of the temperature output registers*/
    uint8_t REG_TEMP_OUT_H = 0x39;
    uint8_t REG_TEMP_OUT_L = 0x3A;

    /*Set the read/write array to be {0, 1} meaning {write, read}. This format is outlined in the datasheet - 
    the IMU expects the register address you want to read from to be written to it, then the next read request 
    the IMU will return the contents of that register
    */
    rw[0] = 0;
    rw[1] = 1;

    command[0] = REG_TEMP_OUT_H;    //Set the write command to be the address of TEMP_OUT_H
    I2C.writeReadBus(addr, 2, command, rw); //Perform the write/read operation. The read byte will be stored in index 1 of the buffer

    *t = (uint16_t)command[1] << 8; //Set the MSB of the temperature reading by output the reading to the memory address provided in the parameters

    command[0] = REG_TEMP_OUT_L;    //Set the write command to be the address of TEMP_OUT_L
    I2C.writeReadBus(addr, 2, command, rw); //Perform the write/read operation. The read byte will be stored in index 1 of the buffer

    *t |= (uint16_t)command[1]; //Set the LSB of the temperature reading by output the reading to the memory address provided in the parameters
}

void imu::changeBank(uint8_t bank){
    /*changeBank(uint8_t bank) - This is a method to change the current register bank.  The IMU has 4 register banks and the bank
        needs to be changed to access different output and configuration registers.

        Parameters:
            bank    -   integer 0-3 corresponding to which register bank is being selected
    */

    uint8_t REG_ADDR = 0x7F;    //Store the address of the register used to change the current bank - Note this is the same address in all 4 banks

    /*Only update the bank if the desired bank is not the current bank -- reduces unnecessary transactions*/
    if(currentBank != bank){
        currentBank = bank;
        uint8_t command[2] = {REG_ADDR, bank << 4};
        I2C.writeBus(addr, 2, command); //Write the buffer to the IMU to change the current bank
    } 
}

uint8_t imu::whoami(){
    /*whoami()  -   This is a method to return the ID from the WHOAMI register. This is likely not useful, but was for debugging issues
        and ensuring that reads from the IMU were correct.  The device should always return a value of 0xEA (234)

        Return: Contents of the WHOAMI register which should always be 0xEA (234)
    */

    changeBank(0x00);   //Change to register bank 0
    uint8_t command[2]; //Declare vairable for the read/write buffer
    bool rw[2]; //Declare variable to hold whether command is a read or write

    uint8_t REG_WHOAMI = 0x00;  //Save the address of the WHOAMI register

    /*Set the read/write array to be {0, 1} meaning {write, read}. This format is outlined in the datasheet - 
    the IMU expects the register address you want to read from to be written to it, then the next read request 
    the IMU will return the contents of that register
    */
    rw[0] = 0;
    rw[1] = 1;

    command[0] = REG_WHOAMI;    //Set the write command data to be the address of the WHOAMI register
    I2C.writeReadBus(addr, 2, command, rw); //Perform the write/read operation. The read byte will be stored in Index 1 of the buffers

    return command[1];  //Return the read byte
}

void imu::checkConfig(){
    /*checkConfig() -   This is a method used to read the configuration registers set with the update_XXXX() functions
        This is useful for debugging to ensure that the device is configured as expected
        
        This function will simply read each register and write the value to the terminal so that the user can ensure the
        values are what they expect*/

    printf("Checking CONFIG registers...\n");

    uint8_t command[2]; //Declare variable for the read/write buffer
    bool rw[2] = {0, 1};    //Declare variable to store whether command is read or write - Initialize so the first command is write and then read

    changeBank(0x00);   //Change to register bank 0

    /*Store the addresses of the config registers in bank 0*/
    uint8_t REG_USER_CTRL = 0x03;

    uint8_t REG_PWR_MGMT_1 = 0x06;
    uint8_t REG_PWR_MGMT_2 = 0x07;

    /*This section performs the write/read command on each of the registers, and displays the reading to the terminal*/
    command[0] = REG_USER_CTRL;
    I2C.writeReadBus(addr, 2, command, rw);
    printf("\nUSER_CTRL:\t%d\n", command[1]);

    command[0] = REG_PWR_MGMT_1;
    I2C.writeReadBus(addr, 2, command, rw);
    printf("\nPWR_MGMT_1:\t%d\n", command[1]);

    command[0] = REG_PWR_MGMT_2;
    I2C.writeReadBus(addr, 2, command, rw);
    printf("\nPWR_MGMT_2:\t%d\n", command[1]);
    
    changeBank(0x02);   //Change to register bank 2

    /*Store the addresses of the config registers in bank 2*/
    uint8_t REG_ACCEL_SMPLRT_DIV_1 = 0x10;
    uint8_t REG_ACCEL_SMPLRT_DIV_2 = 0x11;
    uint8_t REG_ACCEL_INTEL_CTRL = 0x12;
    uint8_t REG_ACCEL_CONFIG_1 = 0x14;
    uint8_t REG_ACCEL_CONFIG_2 = 0x15;

    uint8_t REG_GYRO_SMPLRT_DIV = 0x00;
    uint8_t REG_GYRO_CONFIG_1 = 0x01;
    uint8_t REG_GYRO_CONFIG_2 = 0x02;

    /*This section performs the write/read command on each of the registers, and displays the reading to the terminal*/
    command[0] = REG_ACCEL_SMPLRT_DIV_1;
    I2C.writeReadBus(addr, 2, command, rw);
    printf("ACCEL_SMPLRT_DIV_1:\t%d\n", command[1]);

    command[0] = REG_ACCEL_SMPLRT_DIV_2;
    I2C.writeReadBus(addr, 2, command, rw);
    printf("ACCEL_SMPLRT_DIV_2:\t%d\n", command[1]);

    command[0] = REG_ACCEL_INTEL_CTRL;
    I2C.writeReadBus(addr, 2, command, rw);
    printf("ACCEL_INTEL_CTRL:\t%d\n", command[1]);

    command[0] = REG_ACCEL_CONFIG_1;
    I2C.writeReadBus(addr, 2, command, rw);
    printf("ACCEL_CONFIG_1:\t%d\n", command[1]);

    command[0] = REG_ACCEL_CONFIG_2;
    I2C.writeReadBus(addr, 2, command, rw);
    printf("ACCEL_CONFIG_2:\t%d\n", command[1]);

    command[0] = REG_GYRO_SMPLRT_DIV;
    I2C.writeReadBus(addr, 2, command, rw);
    printf("GYRO_SMPLRT_DIV:\t%d\n", command[1]);

    command[0] = REG_GYRO_CONFIG_1;
    I2C.writeReadBus(addr, 2, command, rw);
    printf("GYRO_CONFIG_1:\t%d\n", command[1]);

    command[0] = REG_GYRO_CONFIG_2;
    I2C.writeReadBus(addr, 2, command, rw);
    printf("GYRO_CONFIG_2:\t%d\n", command[1]);


    uint8_t magAddr = 0x0C; //Store the I2C address of the Magnetometer chip

    /*Store the addresses of the config registers on the magnetometer*/
    uint8_t REG_CNTL2 = 0x31;
    uint8_t REG_CNTL3 = 0x32;

    /*This section performs the write/read command on each of the registers, and displays the reading to the terminal*/
    command[0] = REG_CNTL2;
    I2C.writeReadBus(magAddr, 2, command, rw);
    printf("\nMAG_CNTL2:\t%d\n", command[1]);

    command[0] = REG_CNTL3;
    I2C.writeReadBus(magAddr, 2, command, rw);
    printf("\nMAG_CNTL3:\t%d\n", command[1]);

    printf("\nRegister check complete.\n");
}