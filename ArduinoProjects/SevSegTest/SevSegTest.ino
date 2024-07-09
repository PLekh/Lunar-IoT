#include "SevSeg.h"
SevSeg sevseg;
#define delaytime 1000
unsigned long previousMillis = 0;

#define LEN(arr) ((int) (sizeof (arr) / sizeof (arr)[0]))
#define closeTo(num, check) (num > check -10 & num < check + 10)
#define EmptyCode 0
#define DisplayCode 1
#define PotCode 2
#define NumPorts 1
#define NumPins 13
//#define licr 914
#define licr 92
#define sese 840
#define sensor1 1
#define delaytime 1000
#define bestport 0
const int ports[NumPorts][NumPins] = {
  {2,3,4,5,6,7,8,9,10,11,12,13,0}
};
int begun[1] = {0};

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  
      //Serial.print(printstring);

  /*byte numDigits = 4;
      //byte digitPins[] = {ports[bestport][0], ports[bestport][1], ports[bestport][2], ports[bestport][3]};
      //byte segmentPins[] = {ports[bestport][4], ports[bestport][5], ports[bestport][6], ports[bestport][7], ports[bestport][8], ports[bestport][9], ports[bestport][10], ports[bestport][11]};
  byte digitPins[] = {2, 3, 4, 5};
  byte segmentPins[] = {6, 7, 8, 9, 10, 11, 12, 13};
  bool resistorsOnSegments = false; // 'false' means resistors are on digit pins
  byte hardwareConfig = COMMON_CATHODE; // See README.md for options
  bool updateWithDelays = true; // Default. Recommended
  bool leadingZeros = false;

  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments, updateWithDelays, leadingZeros);
  sevseg.setBrightness(90);*/
}

void loop() {
  int x = 0;
  //Serial.print("checking condition");
  int sensorval1 = analogRead(1);
  char sensorval1str[5];
  sprintf(sensorval1str,"%d", sensorval1);

  unsigned long currentMillis = millis();
  // put your main code here, to run repeatedly:
  if (currentMillis - previousMillis >= 1000) {
    //save the last time you blinked the LED
    previousMillis = currentMillis;
    
      if(x == 0){
        Serial.println("Initializing the seven segment");

        byte numDigits = 4;
        byte digitPins[] = {ports[bestport][0], ports[bestport][1], ports[bestport][2], ports[bestport][3]};
        byte segmentPins[] = {ports[bestport][4], ports[bestport][5], ports[bestport][6], ports[bestport][7], ports[bestport][8], ports[bestport][9], ports[bestport][10], ports[bestport][11]};
        bool resistorsOnSegments = false; // 'false' means resistors are on digit pins
        byte hardwareConfig = COMMON_CATHODE; // See README.md for options
        bool updateWithDelays = false; // Default. Recommended
        bool leadingZeros = false;

        sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments, updateWithDelays, leadingZeros);
        sevseg.setBrightness(90);
        x=1;
      }
      
      Serial.println("Printing on Seven segment");
      Serial.println("The print Value is ");
      Serial.print(sensorval1str);
      sevseg.setChars(sensorval1str);
      
      
    
  }
    sevseg.refreshDisplay();
}

