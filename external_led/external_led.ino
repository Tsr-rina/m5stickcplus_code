#include <M5StickCPlus.h>

void setup() {
  // put your setup code here, to run once:
  M5.begin();
  M5.lcd.setRotation(3);
  pinMode(26, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  int count;
  for (count=1; count <=10; count=count+1){
    M5.lcd.print(count);
    digitalWrite(26, HIGH);
    delay(500);
    digitalWrite(26, LOW);
    delay(500);
  }
  

}
