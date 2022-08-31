#include <M5StickCPlus.h>
#include <WiFi.h>
#include <WebServer.h>
#include <stdlib.h>
#include <time.h>

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
float ax, ay, az;

float x,y,z;

int stamp;
int color;
int c = 0;

int mondai_number = 0;

// 「"」は「\"」に置き換え、htmlの改行は「\n」、コードの改行は「\」
String html = "\
<!DOCTYPE html><html lang=\'jp\'><head>\n\
  <meta charset=\'UTF-8\'><meta name=\'viewport\' content=\'width=device-width, initial-scale=1.0\'>\n\
  <title>PictQuiz</title>\n\
<!-- CSS処理（ブラウザページ装飾）------------------------ -->\n\
  <style>\n\
    h3{text-align:center;}\n\
    .select{margin:10px 100px 10px;}\n\
    #quit{margin-left:200px;margin-top:10px;}\n\
    .push{float:left;text-align:center;margin:10px;background-color:black;color:white;border-radius:\'10px\';}\n\
    #que1{margin-left:300px;}\n\
    #que3{margin-right:10px;}\n\
    .intro{font-family:Comic Sans MS;font-size:40px;text-align:center;border-bottom:double 5px black;}\n\
    .intro p{margin-top:20px;margin-bottom:10px;font-family:\'メイリオ\';}\n\
    #ex{margin-top:0px;font-size:13px;}\n\
    #question{float:left;border:solid 5px black;margin:30px 60px 15px;width:400px;height:400px;visibility: hidden;}\n\
    #fish{display:none;}\n\
    #bath{display:none;}\n\
    #expla1{margin-top:30px;float:left;width:500px;height:300px;}\n\
    #expla{width:500px;height:70px;visibility: hidden;}\n\
    #expla p{text-align:center;}\n\
    #answer1{width:500px;height:70px;text-align:center;margin-left:10%;display:none;}\n\
    #answer1 input{float:left;width:100px;height:50px;margin:20px;}\n\
    #answer2{width:500px;height:70px;text-align:center;margin-left:10%;display:none;}\n\
    #answer2 input{float:left;width:100px;height:50px;margin:20px;}\n\
    #answer3{width:500px; height:70px;text-align:center;margin-left:10%;display:none;}\n\
    #answer3 input{float:left;width:100px;height:50px;margin:20px;}\n\
    #message{margin:0px;width:500px;height:100px;}\n\
    #messa1{margin:5px;width:500px;height:30px;text-align:center;}\n\
    #messa2{margin:10px 5px 10px;width:500px;height:30px;text-align:center;}\n\
    .que_on {background-color: yellow;}\n\
    </style></head>\n\
<!-- html処理（ブラウザ表示）----------------------------- -->\n\
<body>\n\
    <div class='intro'>\n\
        <p>Pictogram Quiz</p>\n\
    </div>\n\
    <br>\n\
    <h1 id=\"count\"></h1>\n\
    <p>xの動き</p>\n\
    <p id=\"x\"></p>\n\
     <p>yの動き</p>\n\
    <p id=\"y\"></p>\n\
     <p>zの動き</p>\n\
    <p id=\"z\"></p>\n\
    <div class='main'>\n\
        <h3>さゆうにうごかして、もんだいをえらんでね</h3>\n\
        <div class='select'>\n\
            <input class='push' id='que1' type='button' value='もんだい1' onClick='Que1()'>\n\
            <input class='push' id='que2' type='button' value='もんだい2' onClick='Que2()'>\n\
            <input class='push' id='que3' type='button' value='もんだい3' onClick='Que3()'>\n\
            <input class='push' id='quit' type='button' value='おしまい' onclick='Quit()'>\n\
        </div>\n\
        <div id='question'>\n\
             <!--もんだい1(魚釣り)-->\n\
            <svg id='fish' width='400px' height='400px' viewbox='0 0 400 400'>\n\
                <circle cx='300' cy='100' r='50' fill='black'></circle><!--あたま-->\n\
                <ellipse cx='300' cy='230' rx='45' ry='80' fill='black'></ellipse> <!--体-->\n\
                <ellipse cx='280' cy='320' rx='12' ry='30' fill='black'></ellipse> <!--左足-->\n\
                <ellipse cx='320' cy='320' rx='12' ry='30' fill='black'></ellipse> <!--右足--> \n\
                <ellipse cx='250' cy='200' rx='35' ry='15' fill='black'></ellipse><!--手-->\n\
                <path d='M80,60 100,60 235,200 230,212 100,60' fill='black'></path><!--釣り竿-->\n\
                <line x1=100 y1=60 x2=100  y2=230 stroke='black'></line><!--釣り糸-->\n\
                <path d='M100,230 S89,270 80,230' fill='white' stroke='black'></path><!--はり-->\n\
                <path d='M50,280 70,300 Q95,340 130,300 Q95,260 70,300 L50,320' fill='black'></path><!--さかな-->\n\
            </svg>\n\
             <!--もんだい2(お風呂)-->\n\
            <svg  id='bath' width='400px' height='400px' viewbox='0 0 400 400'>\n\
                <path d='M50,200 L350,200 330,300 Q320,350 260,350 L140,350 Q80,350 70,300' fill='black'></path><!--お風呂-->\n\
                <circle cx='270' cy='150' r='40' fill='black'></circle><!--あたま-->\n\
                <path d='M235,210 L255,190 Q295,180 270,220 L245,245 Q235,250 230,245 L200,210 Q190,180 220,190 L236,211' stroke='white' stroke-width='2px'></path><!--うで-->\n\
                <path d='M25,340 L25,80 40,80 40,340 45,340 45,345 20,345 20,340'></path><!--シャワー本体-->\n\
                <path d='M25,80 L45,45 55,60 40,80 '></path><!--シャワーヘッド-->\n\
                <circle cx='65' cy='55' r='25' fill='black'></circle><!--シャワーヘッド外-->\n\
                <circle cx='65' cy='55' r='20' fill='white'></circle><!--シャワーヘッド内-->\n\
                <path d='M70,45 L150,90' stroke='black' stroke-dasharray='6 6'></path><!--水(上)-->\n\
                <path d='M60,50 L160,140' stroke='black' stroke-dasharray='6 6'></path><!--水(中央)-->\n\
                <path d='M60,65 L140,150' stroke='black' stroke-dasharray='6 6'></path><!--水(下)-->\n\
            </svg>\n\
            <!--もんだい3(シーソー)-->\n\
            <svg id='seesaw' width='400px' height='400px' viewbox='0 0 400 400'>\n\
                <path d='M180,270 L180,230 Q200,200 220,230 L220,270'></path><!--支点-->\n\
                <path d='M50,250 L350,170 350,180 50,260' fill='black'></path><!--座る場所-->\n\
                <circle cx='85' cy='170' r='20' fill='black'></circle><!--左の頭-->\n\
                <ellipse cx='85' cy='215' rx='20' ry='30' fill='black'></ellipse><!--左の体-->\n\
                <path d='M87,240 Q97,235 107,255 Q97,260 87,240' fill='black'></path><!--左の足-->\n\
                <ellipse cx='108' cy='210' rx='15' ry='5'></ellipse><!--左の手-->\n\
                <path d='M120,205 L125,205 125,230 120,233'></path><!--左の手すり-->\n\
                <circle cx='310' cy='118' r='20' fill='black'></circle><!--右の頭-->\n\
                <ellipse cx='310' cy='160' rx='18' ry='25'></ellipse><!--右の体-->\n\
                <path d='M310,185 Q300,180 290,200 Q300,205 310,185'></path><!--右の足-->\n\
                <ellipse cx='287' cy='164' rx='15' ry='5'></ellipse><!--右の手-->\n\
                <path d='M270,165 275,165 275,190 270,193'></path><!--右の手すり-->\n\
            </svg>\n\
        </div>\n\
        <div id='expla1'>\n\
            <div id='expla'>\n\
            <p id=\"mondai\">ひだりのピクトグラムはなにをあらわしているでしょうか</p>\n\
            <p>ボタンをおしてこたえてね</p>\n\
            </div>\n\
            <br>\n\
            <div class='answer'>\n\
                <div id='answer1'>\n\
                    <input class='push' id='a1-1' type='button' value='①つり' onclick='Ans11()'>\n\
                    <input class='push' id='a1-2' type='button' value='②ヨーヨー' onclick='Ans12()'>\n\
                </div>\n\
                <div id='answer2'>\n\
                    <input class='push' id='a2-1' type='button' value='①ふね' onclick='Ans21()'>\n\
                    <input class='push' id='a2-2' type='button' value='②おふろ' onclick='Ans22()'>\n\
                </div>\n\
                <div id='answer3'>\n\
                    <input class='push' id='a2-1' type='button' value='①シーソー' onclick='Ans31()'>\n\
                    <input class='push' id='a2-2' type='button' value='②ブランコ' onclick='Ans32()'>\n\
                 </div>\n\
            </div>\n\
            <br><br><br>\n\
            <div id='message'>\n\
                <p id='messa1'></p>\n\
                <p id='messa2'></p>\n\
            </div>\n\
        </div>\n\
    </div>\n\
</body>\n\
<!-- JavaScript処理--------------------------------------- -->\n\
  <script type=\'text/javascript\'>\n\
    let que = [];\n\
    const queOn = (i) => {  //ボタンON時処理（ボタン番号「i」ごとに分岐、複数可）\n\
      que[i].classList.add('que_on');\n\
      switch (i) {\n\
        case 0: getBtnOn(i); break;\n\
        case 1: getBtnOn(i); break;\n\
        case 2: getBtnOn(i); break; \n\
      }\n\
    }\n\
    const queOff = (i) => {  //ボタンOFF時処理（ボタン番号「i」ごとに分岐、複数可）\n\
      que[i].classList.add('que_on');\n\
      switch (i) {\n\
        case 0: getBtnOff(i); break;\n\
        case 1: getBtnOff(i); break; \n\
        case 2: getBtnOff(i); break; \n\
      }\n\
    }\n\
    for (let i = 0; i < 3; i++) { //ブラウザボタン状態取得（イベント処理、複数可）※i=btn番号\n\
      que[i] = document.getElementById('que' + (i+1));\n\
      que[i].addEventListener('touchstart', (e) => {e.preventDefault(); queOn(i);} );\n\
      que[i].addEventListener('mousedown', () => {queOn(i);} );\n\
      que[i].addEventListener('touchend', () => {queOff(i);} );\n\
      que[i].addEventListener('mouseup', () => {queOff(i);} );\n\
    }\n\
    let get_data;\n\
    async function getData() {  //マイコンボード側JSONデータ取得（インターバル）\n\
      await fetch(\"/get/data\") //サーバーへリクエストを送る \n\
        .then((response) => {if (response.ok) {return response.json();} else {throw new Error();} })\n\
        .then((json) => {\n\
          console.log(json);\n\
          get_data = json;\n\
          let el;\n\
          const x = document.querySelector('#x');\n\
          x.textContent = get_data.x;\n\
          const y = document.querySelector('#y');\n\
          y.textContent = get_data.y;\n\
          const z = document.querySelector('#z');\n\
          z.textContent = get_data.z;\n\
          if(get_data.y == 139.00){\n\
            c = 0;\n\
            const ct = document.querySelector('#count');\n\
            ct.textContent = get_data.y;\n\
            mondai_number = 2;\n\
            Que2();\n\
            if (get_data.c == 1){\n\
              document.getElementById('mondai').style.fontWeight= 'bold';\n\
              Ans21();\n\
            }else if(get_data.c == 2){\n\
              Ans22();\n\
            }\n\
          }else if(get_data.y == 140.00){\n\
            Que3();\n\
          }else{\n\
            Que1();\n\
          }\n\
          //以下に取得したデータごとに処理したい内容を記入\n\
          count = get_data.c;\n\
          el = document.querySelector('#v_in'); //アナログ電圧表示要素取得(idはJSONのキー)\n\
          el.textContent = get_data.v_in;       //アナログ電圧表示更新\n\
          if (get_data.LED_state == 0) {que[0].classList.add('que_on');}  //LEDがONならブラウザボタン緑\n\
          else {que[0].classList.remove('que_on');}                       //LEDがOFFならブラウザボタン白\n\
        })\n\
        .catch((error) => console.log(error));\n\
    }\n\
    setInterval(getData, 100); //インターバル設定（1秒ごとに本体データ取得）\n\
    \n\
    async function getBtnOn(i) {  //ブラウザボタンON時処理（複数可）\n\
      let link;\n\
      switch (i) {\n\
        case 0: link = \"/get/que0_on\"; break; //ブラウザボタン番号（btn i）ごとに処理を分岐\n\
        case 1: link = \"/get/que1_on\"; break;\n\
        case 2: link = \"/get/que2_on\"; break;\n\
      }\n\
      await fetch(link)\n\
        .then((response) => { if (response.ok) {return response.text();} else {throw new Error();} })\n\
        .then((text) => { console.log(text) })\n\
        .catch((error) => console.log(error));\n\
    }\n\
    async function getBtnOff(i) { //ブラウザボタンOFF時処理（複数可）\n\
      let link;\n\
      switch (i) {\n\
        case 0: link = \"/get/que0_off\"; break;  //ブラウザボタン番号（btn i）ごとに処理を分岐\n\
        case 1: link = \"/get/que1_off\"; break;\n\
        case 2: link = \"/get/que2_off\"; break;\n\
      }\n\
      await fetch(link)\n\
        .then((response) => { if (response.ok) {return response.text();} else {throw new Error();} })\n\
        .then((text) => { console.log(text) })\n\
        .catch((error) => console.log(error));\n\
    }\n\
  function Que1(){\n\
    document.getElementById('question').style.visibility='visible';\n\
    document.getElementById('expla').style.visibility='visible';\n\
    document.getElementById('que1').style.backgroundColor='gray';\n\
    var fi =document.getElementById('fish');\n\
    var a1=document.getElementById('answer1');\n\
    m1=document.getElementById('messa1').innerHTML='';\n\
    m2=document.getElementById('messa2').innerHTML='';\n\
    fi.style.display='block';\n\
    a1.style.display='block';\n\
    if(fi.style.display=='block'){\n\
        document.getElementById('bath').style.display='none';\n\
        document.getElementById('seesaw').style.display='none';\n\
        document.getElementById('answer2').style.display='none';\n\
        document.getElementById('answer3').style.display='none';\n\
        document.getElementById('que2').style.backgroundColor='black';\n\
        document.getElementById('que3').style.backgroundColor='black';\n\
     }\n\
  }\n\
  function Que2(){\n\
    document.getElementById('question').style.visibility='visible';\n\
    document.getElementById('expla').style.visibility='visible';\n\
    document.getElementById('que2').style.backgroundColor='gray';\n\
    var ba =document.getElementById('bath');\n\
    var a2=document.getElementById('answer2');\n\
    m1=document.getElementById('messa1').innerHTML='';\n\
    m2=document.getElementById('messa2').innerHTML='';\n\
    ba.style.display='block';\n\
    a2.style.display='block';\n\
    if(ba.style.display=='block'){\n\
        document.getElementById('fish').style.display='none';\n\
        document.getElementById('seesaw').style.display='none';\n\
        document.getElementById('answer1').style.display='none';\n\
        document.getElementById('answer3').style.display='none';\n\
        document.getElementById('que1').style.backgroundColor='black';\n\
        document.getElementById('que3').style.backgroundColor='black';\n\
     }\n\
  }\n\
  function Que3(){\n\
      document.getElementById('question').style.visibility='visible';\n\
      document.getElementById('expla').style.visibility='visible';\n\
      document.getElementById('que3').style.backgroundColor='gray';\n\
      var se =document.getElementById('seesaw');\n\
      var a3=document.getElementById('answer3');\n\
      m1=document.getElementById('messa1').innerHTML='';\n\
      m2=document.getElementById('messa2').innerHTML='';\n\
      se.style.display='block';\n\
      a3.style.display='block';\n\
      if(se.style.display=='block'){\n\
          document.getElementById('fish').style.display='none';\n\
          document.getElementById('bath').style.display='none';\n\
          document.getElementById('answer1').style.display='none';\n\
          document.getElementById('answer2').style.display='none';\n\
          document.getElementById('que1').style.backgroundColor='black';\n\
          document.getElementById('que2').style.backgroundColor='black';\n\
      }\n\
  }\n\
  function Ans11(){\n\
        document.getElementById('messa1').style.color='red';\n\
        document.getElementById('messa2').style.color='red';\n\
        document.getElementById('messa1').innerHTML='せいかい！ 「つり」だよ';\n\
        document.getElementById('messa2').innerHTML='おめでとう';\n\
   }\n\
   function Ans12(){\n\
        document.getElementById('messa1').style.color='blue';\n\
        document.getElementById('messa2').style.color='blue';\n\
        document.getElementById('messa1').innerHTML='ざんねん。';\n\
        document.getElementById('messa2').innerHTML='もういちどチャレンジしてみよう！';\n\
    }\n\
//    function Ans13(){\n\
//        document.getElementById('messa1').style.color='red';\n\
//        document.getElementById('messa2').style.color='red';\n\
//        document.getElementById('messa1').innerHTML='せいかい！ 「つり」だよ';\n\
//        document.getElementById('messa2').innerHTML='おめでとう';\n\
//    }\n\
    function Ans21(){\n\
        document.getElementById('messa1').style.color='blue';\n\
        document.getElementById('messa2').style.color='blue';\n\
        document.getElementById('messa1').innerHTML='ざんねん。';\n\
        document.getElementById('messa2').innerHTML='もういちどチャレンジしてみよう！';\n\
    }\n\
    function Ans22(){\n\
        document.getElementById('messa1').style.color='red';\n\
        document.getElementById('messa2').style.color='red';\n\
        document.getElementById('messa1').innerHTML='せいかい! 「おふろ」だよ';\n\
        document.getElementById('messa2').innerHTML='おめでとう';\n\
    }\n\
    function Ans23(){\n\
//        document.getElementById('messa1').style.color='blue';\n\
//        document.getElementById('messa2').style.color='blue';\n\
//        document.getElementById('messa1').innerHTML='ざんねん。「ふね」はちがうよ';\n\
//        document.getElementById('messa2').innerHTML='もういちどチャレンジしてみよう！';\n\
    }\n\
    function Ans31(){\n\
        document.getElementById('messa1').style.color='red';\n\
        document.getElementById('messa2').style.color='red';\n\
        document.getElementById('messa1').innerHTML='せいかい! 「シーソー」だよ';\n\
        document.getElementById('messa2').innerHTML='おめでとう';\n\
    }\n\
    function Ans32(){\n\
        document.getElementById('messa1').style.color='blue';\n\
        document.getElementById('messa2').style.color='blue';\n\
        document.getElementById('messa1').innerHTML='ざんねん。';\n\
        document.getElementById('messa2').innerHTML='もういちどチャレンジしてみよう！';\n\
    }\n\
//    function Ans33(){\n\
//        document.getElementById('messa1').style.color='blue';\n\
//        document.getElementById('messa2').style.color='blue';\n\
//        document.getElementById('messa1').innerHTML='ざんねん。「ブランコ」はちがうよ';\n\
//        document.getElementById('messa2').innerHTML='もういちどチャレンジしてみよう！';\n\
//    }\n\
    function Quit(){\n\
      window.location.href='index.html';\n\
    }\n\
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


//ブラウザONボタン処理
void btn1On(){
  btn0_sig = 1;
  server.send(200, "text/plain", "ボタン1 ON");  
}
//ブラウザOFFボタン処理
void btn1Off(){
  btn0_sig = 0;
  server.send(200, "text/plain", "ボタン1 OFF");  
}


//ブラウザONボタン処理
void btn2On(){
  btn0_sig = 1;
  server.send(200, "text/plain", "ボタン2 ON");  
}
//ブラウザOFFボタン処理
void btn2Off(){
  btn0_sig = 0;
  server.send(200, "text/plain", "ボタン2 OFF");  
}

//ブラウザへデータ送信(JSONファイル)
//ここに送信する値を記述
void getData(){
  String data= "";
  //JSONフォーマット（{ 'key（項目）' : 'value（値）' ～ ,'key（項目）' : 'value（値）'}）
  data += "{\"info\":\"";   data += "M5StackC Plus";       //本体情報
  data += "\",\"v_in\":\"";  data += v_in;                 //アナログ入力電圧
  data += "\",\"LED_state\":\"";  data += digitalRead(10); //本体LEDの状態
  data += "\", \"gyroX\":\""; data += gyroX * M5.MPU6886.gRes;
  data += "\", \"gyroY\":\""; data += gyroY * M5.MPU6886.gRes;
  data += "\", \"gyroZ\":\""; data += gyroZ * M5.MPU6886.gRes;
  data += "\", \"stamp\":\""; data += stamp;
  data += "\", \"color\":\""; data += color;
  data += "\", \"c\":\""; data+=c;
  data += "\", \"x\":\""; data+=x;
  data += "\", \"y\":\""; data+=y;
  data += "\", \"z\":\""; data+=z;
  data += "\"}";
  server.send(200, "text/plain", data); 
}

//WiFi接続処理
const char ssid[] = "rs500k-73207a-1";
const char pass[] = "08b84b774d0e8";

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
  WiFiLocal();
  // サーバー設定
  server.on("/", handleRoot);           //ルートアクセス時の応答関数
  server.onNotFound(handleNotFound);    //Webページが見つからない時の応答関数
  server.on("/get/que0_on", btn0On);    //ボタン0オン受信処理
  server.on("/get/que0_off", btn0Off);  //ボタン0オフ受信処理
  server.on("/get/que1_on", btn1On);    //ボタン1オン受信処理
  server.on("/get/que1_off", btn1Off);  //ボタン1オフ受信処理
  server.on("/get/que2_on", btn2On);    //ボタン2オン受信処理
  server.on("/get/que2_off", btn2Off);  //ボタン2オフ受信処理
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
}


void loop() {
  M5.update();                //本体ボタン状態更新（ブザーも更新）
  server.handleClient();      //クライアント（ブラウザ）からのアクセス確認
  // 本体ボタン、外部ボタン、ブラウザボタン入力処理
    //ボタンA または 外部スイッチ赤 または ブラウザボタン0 が押されているなら
  if (M5.BtnA.isPressed()) {
    c = 1;
    digitalWrite(10, LOW);    //本体LED赤点灯
    srand((unsigned int)time(NULL));
    int stamp_r = rand()%3+1;
    stamp = stamp_r;
//    digitalWrite(OUT0, LOW);  //OUT0出力（本体LED赤連動
  }else if (M5.BtnB.isPressed() ){
    srand((unsigned int)time(NULL));
    int color_r = rand()%5+0;
    color = color_r;
    c = 2;
  } else {
    c = 1;
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
//  M5.MPU6886.getGyroData(&gyroX, &gyroY, &gyroZ);
  M5.MPU6886.getAccelData(&ax, &ay, &az);
  int x1 = map((int)ay, 600, 1400, 80, 0);
  int y1 = map((int)az, 600, 1400, 80, 0);
  x = x1;
  y = y1;
  z = az;
  
  M5.Lcd.setCursor(5,65);
//  M5.Lcd.printf("X:%7.2f\nY:%7.2f\nZ:%7.2f",
//              gyroX * M5.MPU6886.gRes,
//              gyroY * M5.MPU6886.gRes,
//              gyroZ * M5.MPU6886.gRes);
  delay(200);   //遅延時間（ms）
}
