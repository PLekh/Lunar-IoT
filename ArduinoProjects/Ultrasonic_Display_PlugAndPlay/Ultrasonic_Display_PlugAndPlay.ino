#include <Arduino.h>


#define LEN(arr) ((int) (sizeof (arr) / sizeof (arr)[0]))
#define closeTo(num, check) (num > check -10 & num < check + 10)

int ports[1][13] = {
  {2,3,4,5,6,7,8,9,10,11,12,13,0}
};

void setup() {
  for (int i; i<LEN(ports); i++){
    Detect(ports[i][0]);
  }
     
}

void loop() {
  
}



