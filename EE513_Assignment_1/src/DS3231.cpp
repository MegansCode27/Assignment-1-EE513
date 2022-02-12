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

#define DS3231_WRITE_ADRS (0x68 << 1)
#define DS3231_READ_ADRS ((0x68 << 1) | 1)
#define ALARM_1_BIT (1)
#define ALARM_2_BIT (2)
#define STATUS_REG_ADRS (0x0F)


#define BUFFER_SIZE 19  //allocates the memory for the OS  //0x00 to 0x12

//states
int file;

//Flags for loop
volatile bool alarm1_flag = false;
volatile bool alarm2_flag = false;

//I2C Bus
I2C i2c(D14, D15);

//DS3231 INT pin connected to D2 with pull-up
InterruptIn rtc_int(D2);

void rtc_isr()
{
    char data[2];
    uint8_t status;
    data[0] = STATUS_REG_ADRS;

    //Set pointer in DS3231
    i2c.write(DS3231_WRITE_ADRS, data, 1);
    //Read status register
    i2c.read(DS3231_READ_ADRS, (data + 1), 1);
    //Save status register in status var
    status = data[1];
    //Clear data[1] for clearing flags in DS3231
    data[1] = 0;
    i2c.write(DS3231_WRITE_ADRS, data, 2);

    if(status & ALARM_1_BIT)
    {
        alarm1_flag = true;
    }

    if(status & ALARM_2_BIT)
    {
        alarm2_flag = true;
    }
}


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



	//Alarms
	  DS3231 rtc(i2c);

	    // !!!**** Read DS3231 datasheet, page 12. ****!!!
	    ds3231_cntl_stat_t rtc_control_status;
	    //Bit2 - !INT/SQW pin used for Interrupts
	    //Bit1 - A2IE set to allow for Alarm2 interrupt
	    //Bit0 - A1IE set to allow for Alarm1 interrupt
	    rtc_control_status.control =  0x07;
	    rtc_control_status.status = 0;

	    //write control, control/status registers
	    rtc.set_cntl_stat_reg(rtc_control_status);

	    //Set time
	    ds3231_time_t rtc_time;
	    rtc_time.seconds = 0;
	    rtc_time.minutes = 0;
	    rtc_time.hours = 0;
	    rtc_time.mode = false; //24Hr mode

	    //write timming registers
	    rtc.set_time(rtc_time);

	    //Set date
	    ds3231_calendar_t rtc_calendar;
	    rtc_calendar.day = 3;
	    rtc_calendar.date = 28;
	    rtc_calendar.month = 2;
	    rtc_calendar.year = 17;

	    //write calendar registers
	    rtc.set_calendar(rtc_calendar);

	    // !!!**** Read DS3231 datasheet, page 12****!!!
	    // !!!**** Table 2 describes am A#M# bits****!!!
	    //Alarm configuration vars
	    ds3231_alrm_t alarm1_config, alarm2_config;

	    //Configure Alarm1 to cause interrupt every minute at 30 second mark
	    alarm1_config.seconds = 30;
	    alarm1_config.minutes = 0;
	    alarm1_config.hours = 0;
	    alarm1_config.day = 1;
	    alarm1_config.date = 1;
	    alarm1_config.am4 = true;
	    alarm1_config.am3 = true;
	    alarm1_config.am2 = true;
	    alarm1_config.am1 = false;
	    alarm1_config.am_pm = false;
	    alarm1_config.mode = false;
	    alarm1_config.dy_dt = true;

	    //write alarm1 registers
	    rtc.set_alarm(alarm1_config, true);

	    //Configure Alarm2 to cause interrupt every minute
	    //Seconds and am1 not used for alarm2,
	    //can only trigger on minutes and longer
	    alarm2_config = alarm1_config;

	    //write alarm2 registers
	    rtc.set_alarm(alarm2_config, false);

	    //Tie D2 falling edge to rtc_isr
	    rtc_int.fall(&rtc_isr);

	    char buffer[32];
	    time_t epoch_time;

	    while(1)
	    {
	        printf(TERM_HOME);

	        //new epoch time fx
	        epoch_time = rtc.get_epoch();

	        printf("\nTime as seconds since January 1, 1970 = %d\n", epoch_time);

	        printf("\nTime as a basic string = %s", ctime(&epoch_time));

	        strftime(buffer, 32, "%I:%M %p\n", localtime(&epoch_time));
	        printf("\nTime as a custom formatted string = %s\n\n", buffer);

	        printf(TERM_CLEAR_FROM_CURSOR);

	        if(alarm1_flag)
	        {
	            alarm1_flag = false;
	            printf("\n!!! ALARM 1 Triggered !!!\n");
	            wait(5.0);
	        }

	        if(alarm2_flag)
	        {
	            alarm2_flag = false;
	            printf("\n*** ALARM 2 Triggered ***\n");
	            wait(5.0);
	        }
	    }







   close(file);
   return 0;
}

