#include <M5StickCPlus.h>
#include <WiFi.h>
#include <WebServer.h>
#include <stdlib.h>
#include <time.h>
#include "BluetoothSerial.h"
BluetoothSerial SerialBT;

//接続名
String name = "BT_P";

//受信データ格納用
String data = "";

//受信データ格納用
String data = "";

//サーバーポート
WebServer server(80);

//入力端子
#define IN0 26
#define IN1 36
//出力端子
#define OUT0 0
//アナログ入力端子
#define ADC0 33

#define MPU6886 IMU

//アナログ入力値格納
float ad_val;

//アナログ入力電圧換算値
float v_in = 0;

//電源ボタン状態取得
int btn_pw = 0;

//バッテリー残量表示
float battery;

//ブラウザボタン0信号
int btn0_sig;

float gyroX, gyroY, gyroZ;

int stamp;
int color;


// 「"」は「\"」に置き換え、htmlの改行は「\n」、コードの改行は「\」
String html = "\
<!DOCTYPE html><html lang=\"jp\"><head>\n\
  <meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n\
  <title>Hidden Drawing</title>\n\
<!-- CSS処理（ブラウザページ装飾）------------------------ -->\n\
  <style>\n\
    body{font-family: sans-serif; background-color: black; max-width: 480px; margin: 0 auto; align-items: center;}\n\
    h1 {color:white; text-align: center; font-size: 28px; margin: 10px auto;}\n\
    h2 {color:white;}\n\
    .info_btn {color:white;}\n\
    div {display: flex; flex-direction: row; justify-content: center; margin-top: 10px;}\n\
    p {margin: 0px;}\n\
    td {padding: 0px 15px; width: 110px; color:#ffffff; text-align: center; font-size: 18px; width: auto;}\n\
    .btn {height: 70px; width: 100px; color: #555555; background-color: #dddde9; font-size: 18px; font-weight: bold; border-radius: 7%; margin: 0 10px; -webkit-appearance: none;}\n\
    .btn_on {background-color: yellow;}\n\
    #canvas {background-color:white;}\n\
  </style></head>\n\
<!-- html処理（ブラウザ表示）----------------------------- -->\n\
<body>\n\
  <h1>M5StickCPlus Canvas</h1>\n\
  <h1 id=\"stamp\"></h1>\n\
  <h2 id=\"color\"></h2>\n\
  <div>\n\
    <!-- ボタンが複数の場合は以下をコピペで増やす。idのbtn番号変更 -->\n\
    <button class=\"btn\" id=\"btn0\">ボタン0</button>\n\
  </div>\n\
  <div id=\"canvas_block\">\n\
    <canvas id=\"canvas\" width=\"700\" height=\"500\">\n\
    </canvas>\n\
  </div>\n\
  <input type=\"text\" id=\"name\">\n\
  <button id=\"submit\">送信する</button>\n\
</body>\n\
<!-- JavaScript処理--------------------------------------- -->\n\
  <script type=\"text/javascript\">\n\
    let btn = [];\n\
    const btnOn = (i) => {  //ボタンON時処理（ボタン番号「i」ごとに分岐、複数可）\n\
      btn[i].classList.add('btn_on');\n\
      switch (i) {\n\
        case 0: getBtnOn(i); break;\n\
      }\n\
    }\n\
    const btnOff = (i) => {  //ボタンOFF時処理（ボタン番号「i」ごとに分岐、複数可）\n\
      btn[i].classList.add('btn_on');\n\
      switch (i) {\n\
        case 0: getBtnOff(i); break;\n\
      }\n\
    }\n\
    for (let i = 0; i < 1; i++) { //ブラウザボタン状態取得（イベント処理、複数可）※i=btn番号\n\
      btn[i] = document.getElementById('btn' + i);\n\
      btn[i].addEventListener('touchstart', (e) => {e.preventDefault(); btnOn(i);} );\n\
      btn[i].addEventListener('mousedown', () => {btnOn(i);} );\n\
      btn[i].addEventListener('touchend', () => {btnOff(i);} );\n\
      btn[i].addEventListener('mouseup', () => {btnOff(i);} );\n\
    }\n\
    let get_data;\n\
    async function getData() {  //マイコンボード側JSONデータ取得（インターバル）\n\
      await fetch(\"/get/data\") //サーバーへリクエストを送る \n\
        .then((response) => {if (response.ok) {return response.json();} else {throw new Error();} })\n\
        .then((json) => {\n\
          console.log(json);\n\
          get_data = json;\n\
          let el;\n\
          //以下に取得したデータごとに処理したい内容を記入\n\
          if (get_data.gyroX >= 1 || get_data.gyroY >= 1 || get_data.gyroZ >= 1){\n\
            const color_ary = [\"red\",\"orange\",\"blue\",\"green\",\"purple\"];\n\
            const canvas = document.querySelector('#canvas');\n\
            const ctx1 = canvas.getContext('2d');\n\
            ctx1.beginPath();\n\
            ctx1.moveTo(get_data.gyroY*50,50);\n\
            ctx1.lineTo(100, get_data.gyroX*50);\n\
            ctx1.lineWidth=20;\n\
            ctx1.strokeStyle = color_ary[get_data.color];\n\
            ctx1.stroke();\n\
          }\n\
          else if (get_data.stamp == 1 || get_data.stamp == 2 || get_data.stamp == 3){\n\
              st = document.querySelector('#stamp');\n\
              st.textContent = get_data.stamp;\n\
              cl = document.querySelector('#color');\n\
              cl.textContent = get_data.color;\n\
              const canvas2 = document.querySelector('#canvas');\n\
              const ctx2 = canvas2.getContext('2d');\n\
              if (get_data.stamp == 1){\n\
                var rX = Math.floor(Math.random()*500)+50;\n\
                var rY = Math.floor(Math.random()*500)+50;\n\
                ctx2.beginPath();\n\
                ctx2.moveTo(rX,rY);\n\
                ctx2.lineTo(rX+25,rY+25);\n\
                ctx2.lineTo(rX+50,rY);\n\
                ctx2.lineWidth=80;\n\
                ctx2.lineCap=\"round\";\n\
                ctx2.strokeStyle=\"orange\";\n\
                ctx2.stroke();\n\
               }\n\
              else if (get_data.stamp == 2){\n\
                var rX = Math.floor(Math.random()*500)+50;\n\
                var rY = Math.floor(Math.random()*300)+50;\n\
                ctx2.beginPath();\n\
                ctx2.moveTo(rX, rY);\n\
                ctx2.lineTo(rX+80, rY);\n\
                ctx2.lineTo(rY, rX+80);\n\
                ctx2.strokeStyle=\"pink\";\n\
                ctx.fill();\n\
                ctx2.stroke();\n\
              }\n\
              else{\n\
                ctx2.strokeStyle=\"yellow\";\n\
                ctx2.rect(500,500, 550, 500);\n\
                ctx2.stroke();\n\
              }\n\
          }\n\
          el = document.querySelector('#v_in'); //アナログ電圧表示要素取得(idはJSONのキー)\n\
          el.textContent = get_data.v_in;       //アナログ電圧表示更新\n\
          if (get_data.LED_state == 0) {btn[0].classList.add('btn_on');}  //LEDがONならブラウザボタン緑\n\
          else {btn[0].classList.remove('btn_on');}                       //LEDがOFFならブラウザボタン白\n\
        })\n\
        .catch((error) => console.log(error));\n\
    }\n\
    setInterval(getData, 100); //インターバル設定（1秒ごとに本体データ取得）\n\
    \n\
    async function getBtnOn(i) {  //ブラウザボタンON時処理（複数可）\n\
      let link;\n\
      switch (i) {\n\
        case 0: link = \"/get/btn0_on\"; break; //ブラウザボタン番号（btn i）ごとに処理を分岐\n\
      }\n\
      await fetch(link)\n\
        .then((response) => { if (response.ok) {return response.text();} else {throw new Error();} })\n\
        .then((text) => { console.log(text) })\n\
        .catch((error) => console.log(error));\n\
    }\n\
    async function getBtnOff(i) { //ブラウザボタンOFF時処理（複数可）\n\
      let link;\n\
      switch (i) {\n\
        case 0: link = \"/get/btn0_off\"; break;  //ブラウザボタン番号（btn i）ごとに処理を分岐\n\
      }\n\
      await fetch(link)\n\
        .then((response) => { if (response.ok) {return response.text();} else {throw new Error();} })\n\
        .then((text) => { console.log(text) })\n\
        .catch((error) => console.log(error));\n\
    }\n\
    document.querySelector('#submit').addEventListener('click', ()=>{ \n\
      console.log('通過1');\n\
      const author = document.querySelector('#name');\n\
      const Hour = now.getHours();\n\
      const Min = now.getMinutes();\n\
      const Sec = now.getSeconds();\n\
      const names = author + Hour + Min + Sec; \n\
      const CLIENT_ID = \"693292035654-bk7s5lv392d8s46lekgal22qr25jh2na.apps.googleusercontent.com\";\n\
      console.log('通過2');\n\
      const SCOPE = \"https://www.googleapis.com/auth/drive/makerfaire\";\n\
      console.log('通過3');\n\
      function handleClientLoad(){window.setTimeout(checkAuth, 1);} \n\
      console.log('通過4');\n\
      function checkAuth(){ gapi.auth.authorize({'client_id':CLIENT_ID, 'scope':SCOPE, 'immediate':true}, handleAuthResult);} \n\
      console.log('通過5');\n\
      function handaleAuthResult(authResult){\n\
        if(authResult && !authResult.error){ \n\
          writeFile();\n\
        }\n\
      }\n\
     function writeFile(evt){ \n\
      gapi.client.load('drive', 'v2', function(){ \n\
        const cs = document.querySelector('#canvas');\n\
        insertFile(names, cs); \n\
      })\n\
     }\n\
     function insertFile(fileName, content, callback){ \n\
      const cotentType = 'image/jpeg'; \n\
      const metadata = {'title':filenName, 'mimeType':contentType};\n\
      const base64Data = utf8_to_b64(content); \n\
      const multipartRequestBody = delimiter + \n\
                'Content-Type: application/json\r\n\r\n' + JSON.stringify(metadata) + delimiter + \n\
                'Content-Type: ' + contentType + '\r\n' + \n\
                'Content-Transfer-Encoding: base64\r\n' + \n\
                '\r\n' + base64Data + close_delim; \n\
       const request = gapi.client.request({ \n\
        'path':'/upload/drive/v2/files',\n\
        'method':'POST', \n\
        'params': { \n\
          'uploadType': 'multipart' \n\
         },\n\
         'headers':{\n\
          'Content-Type': 'multipart/mixed; boundary=\"' + boundary + '\"' \n\
         },\n\
         'body': multipartRequestBody \n\
       });\n\
       if (!callback){ \n\
        callback = function (file){ \n\
          alert('保存しました');\n\
          console.log(file); \n\
        }\n\
       }\n\
       request.execute(callback); \n\
     }\n\
    function utf8_to_b64(img) {return window.btoa( unescape(encodeURIComponent( img )) );} \n\
    )}\n\
  </script></body></html>\n";

//サーバリクエスト時の処理関数
//ルートアクセス時の応答関数
void handleRoot() {
//  htmlData();
  server.send(200, "text/html", html);

}
//エラー(404)の応答関数
void handleNotFound(){
  server.send(404, "text/plain", "404 Not Found!");
}

//ブラウザONボタン処理
void btn0On(){
  btn0_sig = 1;
  server.send(200, "text/plain", "ボタン0 ON");  
}

//ブラウザOFFボタン処理
void btn0Off(){
  btn0_sig = 0;
  server.send(200, "text/plain", "ボタン0 OFF");  
}

//ブラウザへデータ送信(JSONファイル)
//ここに送信する値を記述
void getData(){
  String data= "";
  //JSONフォーマット（{ "key（項目）" : "value（値）" ～ ,"key（項目）" : "value（値）"}）
  data += "{\"info\":\"";   data += "M5StackC Plus";       //本体情報
  data += "\",\"v_in\":\"";  data += v_in;                 //アナログ入力電圧
  data += "\",\"LED_state\":\"";  data += digitalRead(10); //本体LEDの状態
  data += "\", \"gyroX\":\""; data += gyroX * M5.MPU6886.gRes;
  data += "\", \"gyroY\":\""; data += gyroY * M5.MPU6886.gRes;
  data += "\", \"gyroZ\":\""; data += gyroZ * M5.MPU6886.gRes;
  data += "\", \"stamp\":\""; data += stamp;
  data += "\", \"color\":\""; data += color;
  data += "\"}";
  server.send(200, "text/plain", data); 
}

//WiFi接続処理
const char ssid[] = "";
const char pass[] = "";

void WiFiLocal() {
  WiFi.begin(ssid, pass);
//  WiFi接続完了待ち
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.printf("\nSSID:%s\n", ssid);     //接続先SSID表示
  Serial.printf("PASS:%s\n", pass);       //接続パスワード表示
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());         //IPアドレス（配列）
}

//初期設定
void setup(){
  M5.begin();
  Serial.begin(9600);

//  Bluetooth接続開始
  SerialBT.begin(name);
  unit8_t macBT[6];
  esp_read_mac(macBT, ESP_MAC_BT);
  
  
  WiFiLocal();
  // サーバー設定
  server.on("/", handleRoot);           //ルートアクセス時の応答関数
  server.onNotFound(handleNotFound);    //Webページが見つからない時の応答関数
  server.on("/get/btn0_on", btn0On);    //ボタン0オン受信処理
  server.on("/get/btn0_off", btn0Off);  //ボタン0オフ受信処理
  server.on("/get/data", getData);      //ブラウザへのデータ送信処理
  server.begin();                       //Webサーバー開始
  // 入出力ピン設定
  // 入力設定
  pinMode(IN0, INPUT_PULLUP); //入力設定（プルアップ）
  pinMode(IN1, INPUT_PULLUP);
  // 出力設定
  pinMode(OUT0, OUTPUT);      //本体LED赤と連動
  pinMode(10, OUTPUT);        //本体LED赤
  pinMode(2, OUTPUT);         //本体ブザー
  digitalWrite(OUT0, HIGH);   //OUT0初期値OFF（HIGH）
  digitalWrite(10, HIGH);     //本体LED初期値OFF（HIGH）
  // アナログ入力設定
  pinMode(ADC0, ANALOG);      //アナログ入力
  // G36とG25は同時使用不可。使っていない方は以下のようにフローティング入力にする
  gpio_pulldown_dis(GPIO_NUM_25); //G25をフローティング入力に設定
  gpio_pullup_dis(GPIO_NUM_25);
  // 液晶表示初期設定
  M5.Lcd.fillScreen(BLACK); //背景色
  M5.Lcd.setRotation(1);    //画面向き設定（USB位置基準 0：下/ 1：右/ 2：上/ 3：左）
  M5.Lcd.setTextSize(1);    //文字サイズ（整数倍率）
  M5.Lcd.setTextFont(4);    //フォント 1(8px), 2(16px), 4(26px), 6(36px数字-.:apm), 7(7セグ-.:), 8(75px数字-.:)
  M5.MPU6886.Init();

// 電源ONの時のシリアルデータが無くなるまで待つ
  while (Serial.available()) {data = Serial.read();}
}


void loop() {
  M5.update();                //本体ボタン状態更新（ブザーも更新）
  server.handleClient();      //クライアント（ブラウザ）からのアクセス確認
  // 本体ボタン、外部ボタン、ブラウザボタン入力処理
    //ボタンA または 外部スイッチ赤 または ブラウザボタン0 が押されているなら
  if (M5.BtnA.isPressed() || btn0_sig == 1) {
    digitalWrite(10, LOW);    //本体LED赤点灯
    srand((unsigned int)time(NULL));
    int stamp_r = rand()%3+1;
    stamp = stamp_r;
//    digitalWrite(OUT0, LOW);  //OUT0出力（本体LED赤連動
  }else if (M5.BtnB.isPressed() ){
    srand((unsigned int)time(NULL));
    int color_r = rand()%5+0;
    color = color_r; 
  } else {                    //そうでなければ
    digitalWrite(10, HIGH);   //本体LED赤消灯
    stamp = 0;
//    digitalWrite(OUT0, HIGH); //OUT0出力OFF（HIGH）
  }
  // アナログ入力処理
  ad_val = analogRead(ADC0);    //アナログ入力値を取得
  v_in = ad_val * (3.3 / 4095); //3.3Vへ換算
  // バッテリー残量（MAX約4.2V、限界電圧3V）パーセント換算表示
  battery = (M5.Axp.GetBatVoltage() - 3) * 90;  //バッテリー残量取得、換算
  if (battery > 100) { battery = 100; }         //換算値が100以上なら100にする
  // LCD表示処理
  M5.Lcd.setTextFont(2);  //フォント変更
  M5.Lcd.setCursor(5, 5);  M5.Lcd.setTextColor(CYAN, BLACK);    //SSID表示
  M5.Lcd.print("SSID : "); M5.Lcd.print(ssid);
  M5.Lcd.setCursor(5, 20); M5.Lcd.setTextColor(ORANGE, BLACK);  //IPアドレス表示
  M5.Lcd.print("IP        : "); M5.Lcd.print(WiFi.localIP());
  M5.Lcd.setCursor(5, 35);  M5.Lcd.setTextColor(RED, BLACK);    //本体LEDの状態表示
  M5.Lcd.printf("LED_RED   : %d", digitalRead(10));
  M5.Lcd.setCursor(5, 50); M5.Lcd.setTextColor(WHITE, BLACK);   //アナログ入力値表示
  M5.Lcd.printf("ADC : %01.2fv  ( %04.0f )", v_in, ad_val);
  M5.Lcd.setTextFont(2);  //フォント変更
  M5.Lcd.setCursor(200, 118); M5.Lcd.setTextColor(DARKGREY, BLACK); //バッテリー残量表示
  M5.Lcd.printf("%.0f%%    ", battery);
  M5.MPU6886.getGyroData(&gyroX, &gyroY, &gyroZ);
  M5.Lcd.setCursor(5,65);
  M5.Lcd.printf("X:%7.2f\nY:%7.2f\nZ:%7.2f",
              gyroX * M5.MPU6886.gRes,
              gyroY * M5.MPU6886.gRes,
              gyroZ * M5.MPU6886.gRes);
  delay(200);   //遅延時間（ms）
}
