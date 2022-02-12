/*
 * DS3231.cpp
 *
 *  Created on: 8 Feb 2022
 *      Author: megswalsh
 */

#include <iostream> //imports streams needed
using namespace std;
#include<stdio.h> //The stdio.h header defines three variable types,several macros, and various functions for performing input and output.
#include<fcntl.h> // Construct for file control
#include<sys/ioctl.h> // standards for the Kernel
#include<unistd.h> //On Unix-like systems, unistd.h is typically made up largely of system call wrapper functions such as fork, pipe and I/O primitives (read, write, close, etc.).
//#include<linux/i2c-dev.h> // bus interfaceioctl
#define BUFFER_SIZE 19  //allocates the memory for the OS  //0x00 to 0x12


// the time is in the registers in encoded decimal form
int bcdToDec(char b) { return (b/16)*10 + (b%16); }


int main(){
   int file; // creates a integer value File
   printf("Starting the DS3231 test application\n");//messagae to the user
   if((file=open("/dev/i2c-1", O_RDWR)) < 0){
      perror("failed to open the bus\n");
      return 1;
   }
   else {
	   printf("Opened the bus");
   }

  /* if(ioctl(file, I2C_SLAVE, 0x68) < 0){
      perror("Failed to connect to the sensor\n");
      return 1;
   }
   char writeBuffer[1] = {0x00};
   if(write(file, writeBuffer, 1)!=1){
      perror("Failed to reset the read address\n");
      return 1;
   }
   char buf[BUFFER_SIZE];
   if(read(file, buf, BUFFER_SIZE)!=BUFFER_SIZE){
      perror("Failed to read in the buffer\n");
      return 1;
   }*/
   //printf("The RTC time is %02d:%02d:%02d\n", bcdToDec(buf[2]),
        // bcdToDec(buf[1]), bcdToDec(buf[0]));
   //close(file);
  // return 0;
}

