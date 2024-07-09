#include <Arduino.h>
#include <LiquidCrystal.h>
#include "SevSeg.h"

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


int begun[NumPorts] = { };
int codes[NumPorts] = { };
int types[NumPorts] = { };
int used[NumPorts] = { };
int priorities[NumPorts] = { };
const int ports[NumPorts][NumPins] = {
  {2,3,4,5,6,7,8,9,10,11,12,13,0}
};

SevSeg sevseg;

//Serial.println("Initializing the seven segment");


//begin(sese, bestport, 1);

unsigned long previousMillis = 0;

void setup() {
  Serial.begin(9600);
  //delay(1000);
    for (int i; i<LEN(ports); i++){
    Initiate(i);
  }

}

void loop() {
  //unsigned long currentMillis = millis();
  //if (currentMillis - previousMillis >= delaytime) {
    // save the last time you blinked the LED
    //previousMillis = currentMillis;

    int sensorval1 = analogRead(sensor1);
    char sensorval1str[5];
    sprintf(sensorval1str,"%d", sensorval1);
    //Serial.println("Sensor Value is ");
    //Serial.print(sensorval1str);
    ExtDisp(sensorval1str);
    
    /*Serial.println("codes is ");
    Serial.print(codes[0]);
    Serial.println("types is ");
    Serial.print(types[0]);
    Serial.println("priorities is ");
    Serial.print(priorities[0]);
    Serial.println("Display Value is ");
    Serial.println(analogRead(A0));
    memset(used, 0, NumPorts);
    */
  //}

}

void ExtDisp(char printstring[]){
  unsigned long currentMillis = millis();
  /*for(byte i = 0; i <= LEN(types)-1; i++){
    if(types[i] == 1 & codes[i] == licr){
      
    }
    else if(types[i] == 1 & codes[i] == sese){

    }

    else{
      Serial.println("Display not found, printstring is %s", printstring)
    }*/
  if (currentMillis - previousMillis >= 1000) {
    previousMillis = currentMillis;

    int bestport = BestPort(DisplayCode);
    if(codes[bestport] == licr){

      Serial.println("The best port is an LCD");
      Serial.println("The printstring is ");
      Serial.println(printstring);
      //LiquidCrystal lcd(ports[bestport][0], ports[bestport][1], ports[bestport][2], ports[bestport][3]ports[bestport][4], ports[bestport][5]);
      LiquidCrystal lcd(ports[bestport][0], ports[bestport][1], ports[bestport][2], ports[bestport][3], ports[bestport][4], ports[bestport][5]);
      lcd.begin(16,2);
      lcd.print(printstring);
      Serial.println("String printed on LCD");
      used[bestport] = used[bestport]+1;
    }

    else if(codes[bestport] == sese){
      
      if(begun[bestport] != sese){
        Serial.println("Initializing the seven segment");

        byte numDigits = 4;
        byte digitPins[] = {ports[bestport][0], ports[bestport][1], ports[bestport][2], ports[bestport][3]};
        byte segmentPins[] = {ports[bestport][4], ports[bestport][5], ports[bestport][6], ports[bestport][7], ports[bestport][8], ports[bestport] [9], ports[bestport][10], ports[bestport][11]};
        bool resistorsOnSegments = false; // 'false' means resistors are on digit pins
        byte hardwareConfig = COMMON_CATHODE; // See README.md for options
        bool updateWithDelays = false; // Default. Recommended
        bool leadingZeros = false;

        sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments, updateWithDelays, leadingZeros);
        sevseg.setBrightness(90);
        begin(sese, bestport, 1);
      }
      
      Serial.println("Printing on Seven segment");
      Serial.println("The print Value is ");
      Serial.print(printstring);
      sevseg.setChars(printstring);
    }

  else{
    Serial.println("LCD not connected");
  }

  }
  sevseg.refreshDisplay();
}


int BestPort(int typeOfComponent){
  Serial.println("Inside bestPort");
  int highvalue = 0;
  int value = 0;
  int bestport = 0;
  
  for(int i = 0; i <= LEN(types)-1; i++){
    
    if(types[i] == typeOfComponent){
      value = priorities[i] - used[i] * 2;
      
      if(highvalue < value){
        highvalue = value;
        bestport = i;
      }
    }
  }


  Serial.println("The lowest priority value is ");
  Serial.print(highvalue);
  Serial.println("The Best Port is ");
  Serial.print(bestport);
  return bestport;
}

void Initiate(int i){
  Serial.println("Inside Initiate");
  delay(1000);
  //unsigned int detectionVal = analogRead(Port[LEN(Port)-1]);
  unsigned int detectionVal = analogRead(ports[i][12]);
  Serial.println("The detection Port is \n");
  Serial.println(ports[i][12]);
  Serial.println("The Detection value is ");
  Serial.print(detectionVal);
  if(closeTo(detectionVal, licr)){
    codes[i] = licr;
    types[i] = DisplayCode;
    priorities[i] = 0;
    Serial.println("Port has a Liquid Crystal");
  }
  else if(closeTo(detectionVal, sese)){
    codes[i] = sese;
    types[i] = DisplayCode;
    priorities[i] = 1;
    Serial.println("Port has a Seven Segment");
  }
  else{
    codes[i] = 0;
    types[i] = EmptyCode;
    priorities[i] = 254;
    Serial.println("Port is empty");
  }
  //codes[0] == licr;
  /*Serial.println("codes is");
  Serial.println(codes[0]);
  Serial.println("types is");
  Serial.println(types[0]);
  Serial.println("priorities is");
  Serial.println(priorities[0]);*/
}


void begin(int value, int pos, bool replace){
  Serial.println("Inside begin");
  if(replace){
    for(int i=0; i < NumPorts; i++){
      Serial.println("Inside begin");
      if(begun[i] == value){
        begun[i] = 0;
      }
    }
  }
  begun[pos] = value;
}

