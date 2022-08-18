#include <M5StickCPlus.h>
#define MPU6886 IMU

#define SAMPLE_PERIOD 40
#define SAMPLE_SIZE 150 //20ms * 150 = 3秒



void setup() {
  M5.begin();
  M5.Lcd.setRotation(3);
  M5.MPU6886.Init(); //MPU6886を初期設定する

}

float ax, ay, az[SAMPLE_SIZE];

#define X0 5  // 横軸の描画開始座標
#define MINZ 600  // 縦軸の最小値 600mG
#define MAXZ 1400  // 縦軸の最大値 1400mG

void loop() {
  M5.Lcd.fillScreen(BLACK);

  for (int i=0; i<SAMPLE_SIZE; i++) {
    M5.MPU6886.getAccelData(&ax, &ay, &az[i]);

    //mGに変換
    az[i] *= 1000;
    if (i == 0) continue;

    int y0 = map((int)(az[i - 1]), MINZ, MAXZ, M5.Lcd.height(), 0);
    int y1 = map((int)(az[i]), MINZ, MAXZ, M5.Lcd.height(), 0);
    M5.Lcd.drawLine(i - 1 + X0, y0, i + X0, y1, GREEN);

    delay(SAMPLE_PERIOD);
  }
  

}
