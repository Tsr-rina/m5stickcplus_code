#include <M5StickCPlus.h>
#include "BluetoothSerial.h"
BluetoothSerial SerialBT;

//接続名
String name = "BT_P";

//電源ボタン状態取得用
int btn_pw = 0;

//受信データ格納用
String data = "";


void setup() {
  M5.begin();
  Serial.begin(9600);

  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);

  M5.Lcd.setTextFont(2);
  M5.Lcd.println(name);

//  Bluetooth接続開始
  SerialBT.begin(name);

//  MACアドレスの取得と表示
  uint8_t macBT[6];
//  MACアドレス取得
  esp_read_mac(macBT, ESP_MAC_BT);
  M5.Lcd.printf("%02X:%02X:%02X:%02X:%02X:%02X\r\n", macBT[0], macBT[1], macBT[2], macBT[3], macBT[4], macBT[5]);
  Serial.printf("%02X:%02X:%02X:%02X:%02X:%02X\r\n", macBT[0], macBT[1], macBT[2], macBT[3], macBT[4], macBT[5]);

//電源ON時のシリアルデータが無くなるまで待つ
  while (Serial.available()){data = Serial.read();}
  M5.Lcd.setTextFont(4);

}

void loop() {
  M5.update();

//ボタン操作処理(マスター側へデータ送信)
  if(M5.BtnA.wasPressed()){
    SerialBT.print("A ON!\r");
  }
  if(M5.BtnB.wasPressed()){
    SerialBT.print("B ON!\r");
  }

//  データ受信時処理
//Pからの受信データをLCDに表示
  if (SerialBT.available()){
    data = SerialBT.readStringUntil('\r');
    M5.Lcd.print("BT:");
    M5.Lcd.println(data);
    Serial.print(data);
  }
//  シリアル入力データ CR付きで入力をP側へ送信
  if(Serial.available()){
    data = Serial.readStringUntil('\r');
    M5.Lcd.print("SR:");
    M5.Lcd.println(data);
    Serial.print(data);
//    Bluetoothデータ送信
    SerialBT.print(data + "\r");
  }

//  LED点灯処理
  if(data == "A ON!"){
    digitalWrite(10, LOW);
   }
  if(data == "B ON!"){
    digitalWrite(10, HIGH);
  }

//  再起動処理
  if(btn_pw == 2){
    ESP.restart();
  }
//電源ボタン状態取得(1秒以下のONで「2」1秒以上で「1」すぐに「0」に戻る)
  btn_pw = M5.Axp.GetBtnPress();
  delay(20);

}
