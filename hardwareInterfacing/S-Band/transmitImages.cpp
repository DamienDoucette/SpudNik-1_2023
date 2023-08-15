/*
 * This is a test script created to simulate sending an image over the S-band transmitter
 * The plan for the S-Band image transmission is to have a script that will read an image 
 * file from the Raspberry Pi and send that over SPI to the ground.
 * 
 * This script reads an image file from the Raspberry Pi and sends it over the SPI
 * The MOSI and MISO pins are shorted so that the transmitted data is also received
 * The receive buffer is then read and that data is saved to a new image file.
 * This is done to ensure that the data was transfered properly
 */

#include <stdio.h>

/*SPI*/
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

using namespace std;

#define SPI_DEVICE "/dev/spidev0.0" //Select SPI Device
#define tlen 4096   //Define how many bytes to read from file and transmit at a time

char txb[tlen]; //Define transmit buffer
char rxb[tlen]; //Defien receive buffer
uint32_t len_data = tlen;   //Definie the number of bytes to be transmitted at a time
uint32_t spi_speed = 1500000;   //Define the speed of the SPI
struct spi_ioc_transfer spi;    //Initalize structure for the SPI
int response;

int main(){
    /*Set up SPI
     * Assign values previouslt defined to the SPI structure
    */
    spi.tx_buf = (unsigned long)txb;
    spi.rx_buf = (unsigned long)rxb;
    spi.bits_per_word = 0;
    spi.speed_hz = spi_speed;
    spi.delay_usecs = 0;
    spi.len = len_data;

    //Open the file on Raspberry Pi where SPI read/writes occur
    int fd = open(SPI_DEVICE, O_RDWR);
    if(fd < 0) {
        printf("Could not open SPI device.");
        return 1;
    }


    FILE* image;    //Define FILE for the original image
    char* filename = "images_BACKUP/JMiyEP.jpg";    //Save the filename of the original image
    image = fopen(filename, "rb");  //Read the original image in binary

    FILE* out;  //Define FILE for the received image
    out = fopen("out.jpg", "wb");   //Write file for the received image as binary 

    int count = 0;
    //If image is not opened properly
    if(!image){
        printf("File not found.\n");
    } else {
        while(count < 1600000){ //16 000 000 is the number of bytes in the image im transmitting
            /*SEND SPI*/
            fread(txb, 1, tlen, image); //Read tlen number of bytes from image and save them to txb         
            response = ioctl(fd, SPI_IOC_MESSAGE(1), &spi); //Write the TXB to the SPI and read the SPI at the same time
            fwrite(&rxb, 1, tlen, out); //Write tlen number of bytes from RXB to the out image
            //printf("%02x\n", rxb[0]);
            count += tlen;  //Track how many bytes have been transfered
        }
    }
    //printf("File reference: %08x\n", *image);
    printf("Number of bytes: %d\n", count);

    //Close files
    fclose(image);
    fclose(out);
    return 0;
}
