#include <M5StickCPlus.h>

// the setup routine runs once when M5StickC starts up
void setup() {
    M5.begin();
    M5.Lcd.setRotation(3);
}

// the loop routine runs over and over again forever
void loop(){
    // ボタンの状態を更新する関数
    M5.update();
    // カーソル初期化
    M5.Lcd.setCursor(0,0);

    // ホームボタンが現在押されているか
    M5.Lcd.print("BtnA.isPressed();");
    M5.Lcd.println(M5.BtnA.isPressed());

    // ホームボタンが現在離しているか
    M5.Lcd.print("BtnA.isReleased();");
    M5.Lcd.println(M5.BtnA.isReleased());

    if(M5.BtnA.wasPressed()) {
        // 押されたら1
        Serial.println("BtnA.wasPressed() == TRUE ");
    }

    if(M5.BtnA.wasReleased()) {
        // 話しているときは1
        Serial.println("BtnA.wasReleased() == TRUE ");
    }



}