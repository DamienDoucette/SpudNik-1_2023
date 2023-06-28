#include "i2c-lib/i2c.h"
#include <cstdint>



int main(){

	i2c I2C("/dev/i2c-1");
	int addr = 0x68;
	/*
	uint8_t *buffer = I2C.readBus(addr, 3);
	printf("READING");
	for(int i = 0; i < 3; i++){
		printf("%d\t", buffer[i]);
	}
	printf("\n"); */
	
	uint8_t write[60];
	write[0] = 0x00;
	write[1] = 0xAA;
	write[2] = 0x00;
	
	if(I2C.writeBus(addr, 3, write)){
		printf("WRITE FAILED");
	} else {
		printf("Wrote to bus");
	}
	I2C.closeBus();
	return 0;
}
