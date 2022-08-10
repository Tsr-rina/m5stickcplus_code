#include <M5StickCPlus.h>

// the setup routine runs once when M5StickC starts up
void setup() {
  
  // initialize the M5StickC object
  M5.begin();
  M5.Lcd.setRotation(1);
  M5.LcdfillScreen(PINK);
  M5.Lcd.setTextColor(BLACK, WHITE);
  M5.Lcd.setTextFont(4);
  M5.Lcd.setTextSize(2);
  M5.Lcd.print("Hello!!");
  delay(1000);


}

// the loop routine runs over and over again forever
void loop(){
}