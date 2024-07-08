#include "SevSeg.h"
SevSeg sevseg;
#define delaytime 1000
unsigned long previousMillis = 0;

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  Serial.println("Printing on Seven segment");
  Serial.println("The print Value is ");
      //Serial.print(printstring);

  byte numDigits = 4;
      //byte digitPins[] = {ports[bestport][0], ports[bestport][1], ports[bestport][2], ports[bestport][3]};
      //byte segmentPins[] = {ports[bestport][4], ports[bestport][5], ports[bestport][6], ports[bestport][7], ports[bestport][8], ports[bestport][9], ports[bestport][10], ports[bestport][11]};
  byte digitPins[] = {2, 3, 4, 5};
  byte segmentPins[] = {6, 7, 8, 9, 10, 11, 12, 13};
  bool resistorsOnSegments = false; // 'false' means resistors are on digit pins
  byte hardwareConfig = COMMON_CATHODE; // See README.md for options
  bool updateWithDelays = false; // Default. Recommended
  bool leadingZeros = false;

  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments, updateWithDelays, leadingZeros);
  sevseg.setBrightness(90);
}

void loop() {
  unsigned long currentMillis = millis();
  // put your main code here, to run repeatedly:
  if (currentMillis - previousMillis >= delaytime) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    Serial.println("Looping");
    sevseg.setChars("649");

  }
  sevseg.refreshDisplay();
}

