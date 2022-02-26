/*
 * RTC.cpp
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
#include <time.h>
#include <system_error>
#include <ctime>
using namespace std;
#define BUFFER_SIZE 19  //allocates the memory for the OS  //0x00 to 0x12

class RTC {


protected:

	//states
	int addr;
	char buf[BUFFER_SIZE];
	char writeBuffer[1] = { 0x00 };

public:

	RTC(){


		addr=0x68; //address
	}

    //functions
	virtual int bcdToDec(char b) { return (b / 16) * 10 + (b % 16);}
	string display(uint8_t a) {
		stringstream ss;
		ss << setw(3) << (int) a << "(" << bitset<8>(a) << ")";
		return ss.str();
	}


	virtual void writeDate_Time(){


		   // current date/time based on current system
		   time_t now = time(0);
		   tm *ltm = localtime(&now);

	       buf[0] = ltm->tm_sec;//Seconds
	       buf[1] = ltm->tm_min; // Minutes
	       buf[2] = ltm->tm_hour; //Hours

	       printf("The RTC current time is %02d:%02d:%02d\n",buf[2],buf[1],buf[0]);


	       buf[4] = ltm->tm_mday; //Day
	       buf[5] = 1+ ltm->tm_mon; // Month
	       buf[6] = (1900+ltm)->tm_year; // Year

	      printf("The RTC current Date is  %d:%02d:%02d\n", buf[4],buf[5],buf[6]);

	}

	virtual void SetAlarm_1(){

		   time_t now = time(0);
		   tm *ltm = localtime(&now);

	       buf[7] = ltm->tm_sec;//Seconds
	       buf[8] = 30+ltm->tm_min; // Minutes
	       buf[9] = 2+ltm->tm_hour; //Hours
	       buf[0]= ltm->tm_mday;

	       printf("The RTC Alarm is set for  %d-%02d-%04\n:");

	}


};

// the time is in the registers in encoded decimal form

int main() {

    int file;
    RTC rtc;//create object of the class
    int addr=0x68;

	// creates a integer value File
	printf("Starting the DS3231 test application\n"); //messagae to the user


	if ((file = open("/dev/i2c-1", O_RDWR)) < 0) {
				perror("failed to open the bus\n");
			} else {
				printf("Opened the bus\n");
			}

	if (ioctl(file, I2C_SLAVE,addr) < 0) {
		perror("Failed to connect to the sensor\n");
		return 1;
	} else {
		printf("Connected to the DS3231 RTC Clock\n ");
	}

	char writeBuffer[1] = { 0x00 };
	if (write(file, writeBuffer, 1) != 1) {
		perror("Failed to reset the read address\n");
		return 1;
	}

	char buf[BUFFER_SIZE];
	if (read(file, buf, BUFFER_SIZE) != BUFFER_SIZE) {
		perror("Failed to read in the buffer\n");
		return 1;
	}

    //DS3231 Times on startup

	printf("The RTC time is %02d:%02d:%02d\n", rtc.bcdToDec(buf[2]),
			rtc.bcdToDec(buf[1]), rtc.bcdToDec(buf[0]));

	printf("The RTC date is %02d:%02d:%04d\n", rtc.bcdToDec(buf[4]),
			rtc.bcdToDec(buf[5]), rtc.bcdToDec(buf[6]));

	//Returns the temp

	int addrTemp=0x11;
    int addrTempLow=0x12;
	if (ioctl(file, I2C_SLAVE,(addrTemp && addrTempLow)) < 0) {
	perror("Failed to connect to the sensor\n");
	return 1;
	}
	else{


   printf("Temperature is %02d degress \n",(buf[addrTemp]),(buf[addrTempLow]));

	}

	rtc.writeDate_Time();//set time and date
	rtc.SetAlarm_1();

	//Alarms



	close(file);
	return 0;
}
