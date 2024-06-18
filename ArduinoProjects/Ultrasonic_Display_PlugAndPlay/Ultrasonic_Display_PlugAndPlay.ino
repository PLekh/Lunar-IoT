//www.elegoo.com
//2016.12.08
#include "SR04.h"
#include <LiquidCrystal.h>

LiquidCrystal lcd(0, 1, 2, 3, 4, 5);

#define TRIG_PIN 12
#define ECHO_PIN 13
SR04 sr04 = SR04(ECHO_PIN,TRIG_PIN);
long a;

void setup() {
  pinMode(11, OUTPUT);
  digitalWrite(11, HIGH);
   Serial.begin(9600);
   delay(1000);
   lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Hello, World!");
   
}

void loop() {
   a=sr04.Distance();
   Serial.print(a);
   Serial.println("cm");
  
  lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  lcd.print(millis() / 1000);

   delay(1000);
}
