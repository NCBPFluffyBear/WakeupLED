/**
 * A script that changes RGB Strip light colors
 * based on the time of day. Utilizes an IR Transmitter
 * to broadcast signals and a RTC Module to keep
 * track of time, even if wall power is missing.
 * 
 * Coded for Arduino
 * Transmitter on pin 9
 * RTC on default pins
 * 
 * @author NCBPFluffyBear
 */

#include <IRLibAll.h>
#include <Wire.h>
#include "RTClib.h"

#define ON 0
#define MORN 1
#define LEAVE 2

// Store every hour/minute as a constant to prevent calculations/decimal operations
typedef struct cycle_t {
  unsigned int onH;
  unsigned int onM;
  unsigned int warnH;
  unsigned int warnM;
  unsigned int offH;
  unsigned int offM;
} Cycle;

const unsigned int numDays = 5;
const Cycle cycle[numDays] = {
  {8,00,  8,30,  9,00}, // Monday
  {8,30,  9,00,  9,30}, // Tuesday
  {8,00,  8,30,  9,00}, // Wednesday
  {8,30,  9,00,  9,30}, // Thursday
  {11,00,  11,30,  12,00} // Friday
};

const uint32_t PWR = 16712445;
const uint32_t BLUE = 16767015;
const uint32_t ORANGE = 16722645;

RTC_DS3231 myRTC;
IRsend mySender;
const int RECEIVER = 2;
const int TRANSMITTER = 3;

void setup() {
  Serial.begin(9600);
  pinMode(RECEIVER, INPUT);
  pinMode(TRANSMITTER, OUTPUT);
  
  if (!myRTC.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  Wire.begin();
  delay(500);
  Serial.println("Ready!"); 
}

void loop() {
    delay(1000);
    DateTime now = myRTC.now();
    int hour = now.hour();
    int minute = now.minute();
    int day = now.dayOfTheWeek();

    if (day > 0 && day < 6) { // Only accept weekdays
       day--; // Shift to meet struct indexes
       if (hour == cycle[day].onH && minute == cycle[day].onM) {
           mySender.send(NEC, PWR, 32);
           delay(100);
           mySender.send(NEC, BLUE, 32);
           delay(60000);
       } else if (hour == cycle[day].warnH && minute == cycle[day].warnM) {
           mySender.send(NEC, ORANGE, 32);
           delay(60000);
       } else if (hour == cycle[day].offH && minute == cycle[day].offM) {
           mySender.send(NEC, PWR, 32);
           delay(60000);
       }
    }
}
