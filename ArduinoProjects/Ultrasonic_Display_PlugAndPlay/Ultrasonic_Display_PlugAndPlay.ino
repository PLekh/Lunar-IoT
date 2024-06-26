#include <Arduino.h>
#include <LiquidCrystal.h>

#define LEN(arr) ((int) (sizeof (arr) / sizeof (arr)[0]))
#define closeTo(num, check) (num > check -10 & num < check + 10)
#define EmptyCode 0
#define DisplayCode 1
#define DistanceCode 2
#define NumPorts 1
#define NumPins 13
#define licr 914
#define sese 100
#define sensor1 1
#define delaytime = 1000


byte codes[NumPorts] = { };
byte types[NumPorts] = { };
byte used[NumPorts] = { };
byte priorities[NumPorts] = { };
const byte ports[NumPorts][NumPins] = {
  {2,3,4,5,6,7,8,9,10,11,12,13,0}
};

unsigned long previousMillis = 0;

void setup() {
  delay(1000);
    for (byte i; i<LEN(ports); i++){
    Initiate(ports[i], i);
  }
  
  Serial.begin(9600);
}

void loop() {
  unsigned long currentMillis = millis();
if (currentMillis - previousMillis >= delaytime) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    int sensorval1 = analogRead(sensor1);
  char sensorval1str[5];
  sprintf(sensorval1str,"%d", sensorval1);
  //Serial.println(sensorval1str);
  ExtDisp(sensorval1str);
    
  }

}

void ExtDisp(printstring){

  /*for(byte i = 0; i <= LEN(types)-1; i++){
    if(types[i] == 1 & codes[i] == licr){
      
    }
    else if(types[i] == 1 & codes[i] == sese){

    }

    else{
      Serial.println("Display not found, printstring is %s", printstring)
    }*/
    byte bestport = BestPort(DisplayCode)
    if(code[bestport] == licr){
        LiquidCrystal lcd(ports[bestport][0], ports[bestport][1], ports[bestport][2]
        , ports[bestport][3]ports[bestport][4], ports[bestport][5]);

        lcd.begin(16,2);
        lcd.print(printstring);
    }

  }

void BestPort(int typeOfComponent){
  int highvalue = 0;
  int value = 0
  byte bestport = 0;
  
  for(byte i = 0; i <= LEN(types)-1; i++){
    
    if(type[i] == typeOfComponent){
      value = priorities[i] - used[i] * 2
      
      if(highvalue < value){
        highvalue = value
        bestport = i;
      }
    }
  }
  return bestport
}

}

void Initiate(byte Port[], int i){

  unsigned int detectionVal = analogRead(Port[LEN(Port)-1]);

  if(closeTo(detectionVal, licr)){
    codes[i] = licr;
    types[i] = DisplayCode;
    priorities[i] = 0;
    Serial.println("Port %d has a Liquid Crystal", i);
  }
  else if(closeTo(detectionVal, sese)){
    codes[i] = sese;
    types[i] = DisplayCode;
    priorities[i] = 1;
    Serial.println("Port %d has a Seven Segment", i);
  }
  else{
    codes[i] = 0;
    types[i] = EmptyCode;
    priorities[i] = 254;
    Serial.println("Port %d is empty", i);
  }
}
