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


byte codes[NumPorts] = { };
byte types[NumPorts] = { };
byte priorities[NumPorts] = { };
const byte ports[NumPorts][NumPins] = {
  {2,3,4,5,6,7,8,9,10,11,12,13,0}
};

void setup() {
  for (byte i; i<LEN(ports); i++){
    Initiate(ports[i], i);
  }



}

void loop() {

}

void Initiate(byte Port[], int i){

  unsigned int detectionVal = analogRead(Port[LEN(Port)-1]);

  if(closeTo(detectionVal, licr)){
    codes[i] = licr;
    types[i] = DisplayCode;
    priorities[i] = 0;
  }
  else if(closeTo(detectionVal, sese)){
    codes[i] = sese;
    types[i] = DisplayCode;
    priorities[i] = 1;
  }
  else{
    codes[i] = 0;
    types[i] = EmptyCode;
    priorities[i] = 254;
  }
}
