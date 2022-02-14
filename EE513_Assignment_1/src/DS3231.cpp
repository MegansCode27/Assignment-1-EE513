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
#include<linux/i2c-dev.h> // bus interfaceioctl
#include<stdint.h>
#include<bitset>
#include<sstream>
#include<iomanip>
using namespace std;
#include "Wire.h"
#define DS3231_I2C_ADDRESS 0x68
#define BUFFER_SIZE 19  //allocates the memory for the OS  //0x00 to 0x12

//states
int file;


// the time is in the registers in encoded decimal form
int bcdToDec(char b) {

	return (b/16)*10 + (b%16);

}
string display(uint8_t a) {
   stringstream ss;
   ss << setw(3) << (int)a << "(" << bitset<8>(a) << ")";
   return ss.str();
}


int main(){
   // creates a integer value File
   printf("Starting the DS3231 test application\n");//messagae to the user
   if((file=open("/dev/i2c-1", O_RDWR)) < 0){
      perror("failed to open the bus\n");
      return 1;
   }
   else {
	   printf("Opened the bus\n");
   }

   int addr =0x68; // The Address to communicate

   if(ioctl(file, I2C_SLAVE, addr) < 0){
   perror("Failed to connect to the sensor\n");
   return 1;
   }
   else
   {
	printf("Connected to the DS3231 RTC Clock\n ");
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
   }
   printf("The RTC time is %02d:%02d:%02d\n", bcdToDec(buf[2]),
    bcdToDec(buf[1]), bcdToDec(buf[0]));

   printf("The RTC date is %02d:%02d:%02d\n", bcdToDec(buf[4]),
      bcdToDec(buf[5]), bcdToDec(buf[6]));


   //Temp

	int addrTemp = 0x11; // The Address to communicate
	int addrTempLow = 0x12;

	if (ioctl(file, I2C_SLAVE, addrTemp) < 0) {
		perror("Failed to connect to the sensor\n");
		return 1;
	} else {
		//cout << "0x11 & 0x12  (AND) is " << display(addrTemp & addrTempLow) << endl;
		printf("Temperature is %2d degress \n", bcdToDec(buf[addrTemp]));
	}

	//Update date and time to current


	int seconds = 0x00;
	int minutes = 0x01;
	int hours = 0x02;
	int day=0x04;
	int month=0x05;
	int year=0x06;


	if (ioctl(file, I2C_SLAVE, seconds,minutes,hours,day,month,year) < 0) {
		perror("Failed to connect to the sensor\n");
		return 1;
	} else {
		//cout << "0x11 & 0x12  (AND) is " << display(addrTemp & addrTempLow) << endl;

		printf("Set RTC current date to %02d:%02d:%02d\n", bcdToDec(buf[day]),
		      bcdToDec(buf[month]), bcdToDec(buf[year]));
	}
	;

   close(file);
   return 0;
}

