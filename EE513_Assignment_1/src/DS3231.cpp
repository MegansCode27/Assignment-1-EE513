/*****************************************************************************
 * Type : C++
 * File : DS3231.cpp
 * Dec. : DS3231 + AT24C32 IIC Module Precision RTC Module Memory Module
 * Copyright (c) 2013-2014, Bird Techstep, tbird_th@hotmail.com
 *
 * Remark Original codr from DS3231 Library [Arduino]
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *****************************************************************************/
#include "DS3231.h"
#define CLOCK_ADDRESS  0x68
#define ADDR  char(CLOCK_ADDRESS<<1)
#define SINCE 1900

// Constructor
DS3231::DS3231(PinName sda_pin, PinName scl_pin) : _i2c(sda_pin, scl_pin) {
    _i2c.frequency(100000);
}
/*****************************************
    Public Functions
 *****************************************/

void DS3231::getTime(uint8_t& year, uint8_t& month, uint8_t& date, uint8_t& DoW, uint8_t& hour, uint8_t& minute, uint8_t& second) {
    uint8_t tempBuffer;
    bool PM;
    bool h12;

    char data[7];
    char cmd;
    cmd = 0x00;
    _i2c.write(ADDR, &cmd, 1 );
    _i2c.read( ADDR, data, 7 );

    second     = bcdToDec(data[0]);
    minute     = bcdToDec(data[1]);
    tempBuffer = bcdToDec(data[2]);

    h12        = tempBuffer & 0x40;
    if (h12) {
        PM   = tempBuffer & 0x20;
        hour = bcdToDec(tempBuffer & 0x1F);
    } else {
        hour = bcdToDec(tempBuffer & 0x3F);
    }
    DoW   = bcdToDec(data[3]);
    date  = bcdToDec(data[4]);
    month = bcdToDec(data[5] & 0x7F);
    year  = bcdToDec(data[6]);
}

int8_t DS3231::getSecond(void) {
    char data;
    char cmd;
    cmd = 0x00;
    _i2c.write(ADDR, &cmd, 1 );
    _i2c.read( ADDR, &data, 1 );
    return(bcdToDec(data));
}

uint8_t DS3231::getMinute(void) {
    char data;
    char cmd;
    cmd = 0x01;
    _i2c.write(ADDR, &cmd, 1 );
    _i2c.read( ADDR, &data, 1 );
    return(bcdToDec(data));
}

uint8_t DS3231::getHour(bool& h12, bool& PM) {
    char temp_buffer;
    char hour;
    char cmd;
    cmd = 0x02;
    _i2c.write(ADDR, &cmd, 1 );
    _i2c.read( ADDR, &temp_buffer, 1 );

    h12 = temp_buffer & 0x40;
    if (h12) {
        PM = temp_buffer & 0x20;
        hour = bcdToDec(temp_buffer & 0x1F);
    } else {
        hour = bcdToDec(temp_buffer & 0x3F);
    }
    return hour;
}

uint8_t DS3231::getDoW(void) {
    char data;
    char cmd;
    cmd = 0x03;
    _i2c.write(ADDR, &cmd, 1 );
    _i2c.read( ADDR, &data, 1 );
    return(bcdToDec(data));
}

uint8_t DS3231::getDate(void) {
    char data;
    char cmd;
    cmd = 0x04;
    _i2c.write(ADDR, &cmd, 1 );
    _i2c.read( ADDR, &data, 1 );
    return(bcdToDec(data));
}

uint8_t DS3231::getMonth(bool& Century) {
    char temp_buffer;
    char cmd;
    cmd = 0x05;
    _i2c.write(ADDR, &cmd, 1 );
    _i2c.read( ADDR, &temp_buffer, 1 );

    Century = temp_buffer & 0x80;
    return (bcdToDec(temp_buffer & 0x7F)) ;
}

uint8_t DS3231::getYear(void) {
    char data;
    char cmd;
    cmd = 0x06;
    _i2c.write(ADDR, &cmd, 1 );
    _i2c.read( ADDR, &data, 1 );
    return(bcdToDec(data));
}

void DS3231::setSecond(uint8_t Second) {
    // Sets the seconds
    // This function also resets the Oscillator Stop Flag, which is set
    // whenever power is interrupted.
    char cmd[2];
    cmd[0] = 0x00;
    cmd[1] = decToBcd(Second);
    _i2c.write(ADDR, cmd, 2);

    // Clear OSF flag
    uint8_t temp_buffer = readControlByte(1);
    writeControlByte((temp_buffer & 0x7F), 1);
}

void DS3231::setMinute(uint8_t Minute) {
    // Sets the minutes
    char cmd[2];
    cmd[0] = 0x01;
    cmd[1] = decToBcd(Minute);
    _i2c.write(ADDR, cmd, 2);
}

void DS3231::setHour(uint8_t Hour) {
    // Sets the hour, without changing 12/24h mode.
    // The hour must be in 24h format.

    bool h12;

    // Start by figuring out what the 12/24 mode is
    char data;
    char cmd;
    cmd = 0x02;
    _i2c.write(ADDR, &cmd, 1 );
    _i2c.read( ADDR, &data, 1 );

    h12 = (data & 0x40);
    // if h12 is true, it's 12h mode; false is 24h.

    if (h12) {
        // 12 hour
        if (Hour > 12) {
            Hour = decToBcd(Hour-12) | 0x60;
        } else {
            Hour = decToBcd(Hour) & 0xDF;
        }
    } else {
        // 24 hour
        Hour = decToBcd(Hour) & 0xBF;
    }
    char cmdHour[2];
    cmdHour[0] = 0x02;
    cmdHour[1] = Hour;
    _i2c.write(ADDR, cmdHour, 2);
}

void DS3231::setDoW(uint8_t DoW) {
    // Sets the Day of Week
    char cmd[2];
    cmd[0] = 0x03;
    cmd[1] = decToBcd(DoW);
    _i2c.write(ADDR, cmd, 2 );
}

void DS3231::setDate(uint8_t Date) {
    // Sets the Date
    char cmd[2];
    cmd[0] = 0x04;
    cmd[1] = decToBcd(Date);
    _i2c.write(ADDR, cmd, 2 );
}

void DS3231::setMonth(uint8_t Month) {
    // Sets the month
    char cmd[2];
    cmd[0] = 0x05;
    cmd[1] = decToBcd(Month);
    _i2c.write(ADDR, cmd, 2 );
}

void DS3231::setYear(uint8_t Year) {
    // Sets the year
    char cmd[2];
    cmd[0] = 0x06;
    cmd[1] = decToBcd(Year);
    _i2c.write(ADDR, cmd, 2 );
}

void DS3231::setClockMode(bool h12) {
    // sets the mode to 12-hour (true) or 24-hour (false).
    // One thing that bothers me about how I've written this is that
    // if the read and right happen at the right hourly millisecnd,
    // the clock will be set back an hour. Not sure how to do it better,
    // though, and as long as one doesn't set the mode frequently it's
    // a very minimal risk.
    // It's zero risk if you call this BEFORE setting the hour, since
    // the setHour() function doesn't change this mode.

    char temp_buffer;
    char cmd;
    cmd = 0x02;

    // Start by reading byte 0x02.
    _i2c.write(ADDR, &cmd, 1 );
    _i2c.read( ADDR, &temp_buffer, 1 );

    // Set the flag to the requested value:
    if (h12) {
        temp_buffer = temp_buffer | 0x40;
    } else {
        temp_buffer = temp_buffer & 0xBF;
    }

    // Write the byte
    _i2c.write(ADDR, &cmd, 1 );
    _i2c.write(temp_buffer);
}

float DS3231::getTemperature(void) {
    // Checks the internal thermometer on the DS3231 and returns the
    // temperature as a floating-point value.
    char temp[2];
    char cmd;
    cmd = 0x11;
    _i2c.write(ADDR, &cmd, 1);
    _i2c.read( ADDR, temp, 2);
    return float(temp[0]) + 0.25*(temp[1]>>6);
}

void DS3231::getA1Time(uint8_t& A1Day, uint8_t& A1Hour, uint8_t& A1Minute, uint8_t& A1Second, uint8_t& AlarmBits, bool& A1Dy, bool& A1h12, bool& A1PM) {
    uint8_t temp_buffer;
    char data[4];
    char cmd;
    cmd = 0x07;
    _i2c.write(ADDR, &cmd, 1);
    _i2c.read( ADDR, data, 4);

    temp_buffer = data[0];  // Get A1M1 and A1 Seconds
    A1Second    = bcdToDec(temp_buffer & 0x7F);
    // put A1M1 bit in position 0 of DS3231_AlarmBits.
    AlarmBits   = AlarmBits | (temp_buffer & 0x80)>>7;

    temp_buffer = data[1];  // Get A1M2 and A1 minutes
    A1Minute    = bcdToDec(temp_buffer & 0x7F);
    // put A1M2 bit in position 1 of DS3231_AlarmBits.
    AlarmBits   = AlarmBits | (temp_buffer & 0x80)>>6;

    temp_buffer = data[2];  // Get A1M3 and A1 Hour
    // put A1M3 bit in position 2 of DS3231_AlarmBits.
    AlarmBits   = AlarmBits | (temp_buffer & 0x80)>>5;
    // determine A1 12/24 mode
    A1h12       = temp_buffer & 0x40;
    if (A1h12) {
        A1PM    = temp_buffer & 0x20;         // determine am/pm
        A1Hour  = bcdToDec(temp_buffer & 0x1F);   // 12-hour
    } else {
        A1Hour  = bcdToDec(temp_buffer & 0x3F);   // 24-hour
    }

    temp_buffer = data[3];  // Get A1M4 and A1 Day/Date
    // put A1M3 bit in position 3 of DS3231_AlarmBits.
    AlarmBits   = AlarmBits | (temp_buffer & 0x80)>>4;
    // determine A1 day or date flag
    A1Dy        = (temp_buffer & 0x40)>>6;
    if (A1Dy) {
        // alarm is by day of week, not date.
        A1Day   = bcdToDec(temp_buffer & 0x0F);
    } else {
        // alarm is by date, not day of week.
        A1Day   = bcdToDec(temp_buffer & 0x3F);
    }
}

void DS3231::getA2Time(uint8_t& A2Day, uint8_t& A2Hour, uint8_t& A2Minute, uint8_t& AlarmBits, bool& A2Dy, bool& A2h12, bool& A2PM) {
    uint8_t temp_buffer;
    char data[3];
    char cmd;
    cmd = 0x0B;
    _i2c.write(ADDR, &cmd, 1);
    _i2c.read( ADDR, data, 4);

    //I2C.requestFrom(CLOCK_ADDRESS, 3);
    temp_buffer = data[0];  // Get A2M2 and A2 Minutes
    A2Minute    = bcdToDec(temp_buffer & 0x7F);
    // put A2M2 bit in position 4 of DS3231_AlarmBits.
    AlarmBits   = AlarmBits | (temp_buffer & 0x80)>>3;

    temp_buffer = data[1];  // Get A2M3 and A2 Hour
    // put A2M3 bit in position 5 of DS3231_AlarmBits.
    AlarmBits   = AlarmBits | (temp_buffer & 0x80)>>2;
    // determine A2 12/24 mode
    A2h12       = temp_buffer & 0x40;
    if (A2h12) {
        A2PM    = temp_buffer & 0x20;         // determine am/pm
        A2Hour  = bcdToDec(temp_buffer & 0x1F);   // 12-hour
    } else {
        A2Hour  = bcdToDec(temp_buffer & 0x3F);   // 24-hour
    }

    temp_buffer = data[2];  // Get A2M4 and A1 Day/Date
    // put A2M4 bit in position 6 of DS3231_AlarmBits.
    AlarmBits   = AlarmBits | (temp_buffer & 0x80)>>1;
    // determine A2 day or date flag
    A2Dy        = (temp_buffer & 0x40)>>6;
    if (A2Dy) {
        // alarm is by day of week, not date.
        A2Day   = bcdToDec(temp_buffer & 0x0F);
    } else {
        // alarm is by date, not day of week.
        A2Day   = bcdToDec(temp_buffer & 0x3F);
    }
}

void DS3231::setA1Time(uint8_t A1Day, uint8_t A1Hour, uint8_t A1Minute, uint8_t A1Second, uint8_t AlarmBits, bool A1Dy, bool A1h12, bool A1PM) {
    //  Sets the alarm-1 date and time on the DS3231, using A1* information
    uint8_t temp_buffer;

    char data[4];
    data[0] = 0x07; // A1 starts at 07h
    data[1] = decToBcd(A1Second) | ((AlarmBits & 0x01) << 7);  // Send A1 second and A1M1
    data[2] = decToBcd(A1Minute) | ((AlarmBits & 0x02) << 6);  // Send A1 Minute and A1M2

    // Figure out A1 hour
    if (A1h12) {
        // Start by converting existing time to h12 if it was given in 24h.
        if (A1Hour > 12) {
            // well, then, this obviously isn't a h12 time, is it?
            A1Hour = A1Hour - 12;
            A1PM = true;
        }
        if (A1PM) {
            // Afternoon
            // Convert the hour to BCD and add appropriate flags.
            temp_buffer = decToBcd(A1Hour) | 0x60;
        } else {
            // Morning
            // Convert the hour to BCD and add appropriate flags.
            temp_buffer = decToBcd(A1Hour) | 0x40;
        }
    } else {
        // Now for 24h
        temp_buffer = decToBcd(A1Hour);
    }
    temp_buffer = temp_buffer | ((AlarmBits & 0x04)<<5);
    // A1 hour is figured out, send it
    //I2C.write(temp_buffer);
    _i2c.write(temp_buffer);

    // Figure out A1 day/date and A1M4
    temp_buffer = ((AlarmBits & 0x08)<<4) | decToBcd(A1Day);
    if (A1Dy) {
        // Set A1 Day/Date flag (Otherwise it's zero)
        temp_buffer = temp_buffer | 0x40;
    }
    data[3] = temp_buffer;
    _i2c.write(ADDR, data, 4);

}

void DS3231::setA2Time(uint8_t A2Day, uint8_t A2Hour, uint8_t A2Minute, uint8_t AlarmBits, bool A2Dy, bool A2h12, bool A2PM) {
    //  Sets the alarm-2 date and time on the DS3231, using A2* information
    uint8_t temp_buffer;

    char data[4];
    data[0] = 0x0B; // A2 starts at 0Bh
    data[1] = decToBcd(A2Minute) | ((AlarmBits & 0x10) << 3);  // Send A2 Minute and A2M2

    // Figure out A2 hour
    if (A2h12) {
        // Start by converting existing time to h12 if it was given in 24h.
        if (A2Hour > 12) {
            // well, then, this obviously isn't a h12 time, is it?
            A2Hour = A2Hour - 12;
            A2PM = true;
        }
        if (A2PM) {
            // Afternoon
            // Convert the hour to BCD and add appropriate flags.
            temp_buffer = decToBcd(A2Hour) | 0x60;
        } else {
            // Morning
            // Convert the hour to BCD and add appropriate flags.
            temp_buffer = decToBcd(A2Hour) | 0x40;
        }
    } else {
        // Now for 24h
        temp_buffer = decToBcd(A2Hour);
    }
    // add in A2M3 bit
    temp_buffer = temp_buffer | ((AlarmBits & 0x20)<<2);
    // A2 hour is figured out, send it
    data[2] = temp_buffer;

    // Figure out A2 day/date and A2M4
    temp_buffer = ((AlarmBits & 0x40)<<1) | decToBcd(A2Day);
    if (A2Dy) {
        // Set A2 Day/Date flag (Otherwise it's zero)
        temp_buffer = temp_buffer | 0x40;
    }
    data[3] = temp_buffer;
    _i2c.write(ADDR, data, 4);

}

void DS3231::turnOnAlarm(uint8_t Alarm) {
    // turns on alarm number "Alarm". Defaults to 2 if Alarm is not 1.
    uint8_t temp_buffer = readControlByte(0);
    // modify control byte
    if (Alarm == 1) {
        temp_buffer = temp_buffer | 0x05;
    } else {
        temp_buffer = temp_buffer | 0x06;
    }
    writeControlByte(temp_buffer, 0);
}

void DS3231::turnOffAlarm(uint8_t Alarm) {
    // turns off alarm number "Alarm". Defaults to 2 if Alarm is not 1.
    // Leaves interrupt pin alone.
    uint8_t temp_buffer = readControlByte(0);
    // modify control byte
    if (Alarm == 1) {
        temp_buffer = temp_buffer & 0xFE;
    } else {
        temp_buffer = temp_buffer & 0xFD;
    }
    writeControlByte(temp_buffer, 0);
}

bool DS3231::checkAlarmEnabled(uint8_t Alarm) {
    // Checks whether the given alarm is enabled.
    uint8_t result = 0x0;
    uint8_t temp_buffer = readControlByte(0);
    if (Alarm == 1) {
        result = temp_buffer & 0x01;
    } else {
        result = temp_buffer & 0x02;
    }
    return result;
}

bool DS3231::checkIfAlarm(uint8_t Alarm) {
    // Checks whether alarm 1 or alarm 2 flag is on, returns T/F accordingly.
    // Turns flag off, also.
    // defaults to checking alarm 2, unless Alarm == 1.
    uint8_t result;
    uint8_t temp_buffer = readControlByte(1);
    if (Alarm == 1) {
        // Did alarm 1 go off?
        result = temp_buffer & 0x01;
        // clear flag
        temp_buffer = temp_buffer & 0xFE;
    } else {
        // Did alarm 2 go off?
        result = temp_buffer & 0x02;
        // clear flag
        temp_buffer = temp_buffer & 0xFD;
    }
    writeControlByte(temp_buffer, 1);
    return result;
}

void DS3231::enableOscillator(bool TF, bool battery, uint8_t frequency) {
    // turns oscillator on or off. True is on, false is off.
    // if battery is true, turns on even for battery-only operation,
    // otherwise turns off if Vcc is off.
    // frequency must be 0, 1, 2, or 3.
    // 0 = 1 Hz
    // 1 = 1.024 kHz
    // 2 = 4.096 kHz
    // 3 = 8.192 kHz (Default if frequency byte is out of range)
    if (frequency > 3) frequency = 3;
    // read control byte in, but zero out current state of RS2 and RS1.
    uint8_t temp_buffer = readControlByte(0) & 0xE7;
    if (battery) {
        // turn on BBSQW flag
        temp_buffer = temp_buffer | 0x40;
    } else {
        // turn off BBSQW flag
        temp_buffer = temp_buffer & 0xBF;
    }
    if (TF) {
        // set ~EOSC to 0 and INTCN to zero.
        temp_buffer = temp_buffer & 0x7B;
    } else {
        // set ~EOSC to 1, leave INTCN as is.
        temp_buffer = temp_buffer | 0x80;
    }
    // shift frequency into bits 3 and 4 and set.
    frequency = frequency << 3;
    temp_buffer = temp_buffer | frequency;
    // And write the control bits
    writeControlByte(temp_buffer, 0);
}

void DS3231::enable32kHz(bool TF) {
    // turn 32kHz pin on or off
    uint8_t temp_buffer = readControlByte(1);
    if (TF) {
        // turn on 32kHz pin
        temp_buffer = temp_buffer | 0x08;
    } else {
        // turn off 32kHz pin
        temp_buffer = temp_buffer & 0xF7;
    }
    writeControlByte(temp_buffer, 1);
}

bool DS3231::oscillatorCheck() {
    // Returns false if the oscillator has been off for some reason.
    // If this is the case, the time is probably not correct.
    uint8_t temp_buffer = readControlByte(1);
    bool result = true;
    if (temp_buffer & 0x80) {
        // Oscillator Stop Flag (OSF) is set, so return false.
        result = false;
    }
    return result;
}

/*****************************************
    Private Functions
 *****************************************/
uint8_t DS3231::decToBcd(uint8_t val) {
// Convert normal decimal numbers to binary coded decimal
    return ( (val/10*16) + (val%10) );
}

uint8_t DS3231::bcdToDec(uint8_t val) {
// Convert binary coded decimal to normal decimal numbers
    return ( (val/16*10) + (val%16) );
}

uint8_t DS3231::readControlByte(bool which) {
    // Read selected control byte
    // first byte (0) is 0x0e, second (1) is 0x0f
    char data;
    char cmd[2];
    cmd[0] = 0x0F;
    cmd[1] = 0x0E;

    if (which) {
        // second control byte
        _i2c.write(ADDR, &cmd[0], 1 );
    } else {
        // first control byte
        _i2c.write(ADDR, &cmd[1], 1 );
    }
    _i2c.read( ADDR, &data, 1 );
    return data;
}

void DS3231::writeControlByte(uint8_t control, bool which) {
    // Write the selected control byte.
    // which=false -> 0x0e, true->0x0f.
    char data1[2];
    char data2[2];
    data1[0] = 0x0F;
    data1[1] = control;
    data2[0] = 0x0E;
    data2[1] = control;

    if (which) {
        _i2c.read( ADDR, data1, 2);
    } else {
        _i2c.read( ADDR, data2, 2);
    }

}




