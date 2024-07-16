#include <stdint.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#define HARDWARE_TYPE MD_MAX72XX::GENERIC_HW
#define DEVICE_NUMBER 16
#include <WiFi.h>
#include <PubSubClient.h>
#define MQTT_SERVER_IP "mqttgo.io"
#define MQTT_SERVER_PORT 1883
#define MQTT_ID ""
#define MQTT_USERNAME ""
#define MQTT_PASSWORD ""
#include <DHT.h>
#include <ezButton.h>

// 彈跳按鈕
ezButton resetBtn(2);
int btnDelay = 50;
// 迴圈最多跑幾次??
int maxLoop = 2;
// 溫度基準
float balanceTempture = 10;
// 濕度基準
float balanceHumi = 0;
// max7219顯示模式1
uint8_t mqtt1[16][8] = {{255, 255, 255, 255, 255, 255, 255, 255}, {255, 255, 255, 255, 255, 255, 255, 255}, {255, 255, 255, 255, 255, 255, 255, 255}, {255, 255, 255, 255, 255, 255, 255, 255},{255, 255, 255, 255, 255, 255, 255, 255}, {255, 255, 255, 255, 255, 255, 255, 255}, {255, 255, 255, 255, 255, 255, 255, 255}, {255, 255, 255, 255, 255, 255, 255, 255},
{255, 255, 255, 255, 255, 255, 255, 255}, {255, 255, 255, 255, 255, 255, 255, 255}, {255, 255, 255, 255, 255, 255, 255, 255}, {255, 255, 255, 255, 255, 255, 255, 255},{255, 255, 255, 255, 255, 255, 255, 255}, {255, 255, 255, 255, 255, 255, 255, 255}, {255, 255, 255, 255, 255, 255, 255, 255}, {255, 255, 255, 255, 255, 255, 255, 255}};
// max7219顯示模式2
uint8_t mqtt2[16][8] = {{255, 255, 255, 255, 255, 255, 255, 255}, {255, 255, 255, 255, 255, 255, 255, 255}, {255, 255, 255, 255, 255, 255, 255, 255}, {255, 255, 255, 255, 255, 255, 255, 255},{255, 255, 255, 255, 255, 255, 255, 255}, {255, 255, 255, 255, 255, 255, 255, 255}, {255, 255, 255, 255, 255, 255, 255, 255}, {255, 255, 255, 255, 255, 255, 255, 255},
{255, 255, 255, 255, 255, 255, 255, 255}, {255, 255, 255, 255, 255, 255, 255, 255}, {255, 255, 255, 255, 255, 255, 255, 255}, {255, 255, 255, 255, 255, 255, 255, 255},{255, 255, 255, 255, 255, 255, 255, 255}, {255, 255, 255, 255, 255, 255, 255, 255}, {255, 255, 255, 255, 255, 255, 255, 255}, {255, 255, 255, 255, 255, 255, 255, 255}};
// char *max7219Char;
// String max7219Str="";
MD_Parola myDisplay = MD_Parola(HARDWARE_TYPE,4,17,16,DEVICE_NUMBER);
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE,4,17,16,DEVICE_NUMBER);
// wifi 連線資料
char _lwifi_ssid[] = "Gary";
char _lwifi_pass[] = "0972728710";
//char _lwifi_ssid[] = "CMoney";
//char _lwifi_pass[] = "xyz8888888";
// MQTT接收的主題
String receivedTopic="";
// MQTT接收的資料
String receivedMsg="";
// bool waitForE=true;
// bool ended=true;
// bool pubCtrl=false;
WiFiClient mqttClient;
PubSubClient myClient(mqttClient);
DHT dht11_p25(25, DHT11);
// 把MQTT字串轉為 max7219用的資料，所需要的暫存參數
uint8_t myBitmap_max7219[8] ={0};
// 若連不上MQTT，延遲N秒再嘗試
int connectMQTTDelay = 2000;
// 若連不上wifi，延遲N秒再嘗試
int connectWIFIDelay = 500;
// HC-SR501紅外線模組
int infraredPin = 39;
int lastInfraredMode = 0; // 上次運行哪一種模式?  0是紅外線沒偵測到人
int infraedFunc = 1; // 紅外線功能是否開啟
// 馬達功能是否開啟
int motorPin = 33;
int motorFunc = 1;
// LED提示連線PIN
int wifiNotice = 22;
int mqttNotice = 21;
// MQTT 所要連上的Topic
String mqtt1Topic = "max7219-mqtt1";
String mqtt2Topic = "max7219-mqtt2";
String mqtt_Setting = "max7219-setting";
// 
void connectMQTT(bool needDelay = true){
  int impLoop = 0;
  if(needDelay){
    while (!myClient.connected() && impLoop < maxLoop){
      if (!myClient.connect(MQTT_ID,MQTT_USERNAME,MQTT_PASSWORD)){
        delay(connectMQTTDelay);
        impLoop++;
      }
    }
  }
  myClient.connect(MQTT_ID,MQTT_USERNAME,MQTT_PASSWORD);
  myClient.subscribe(String(mqtt1Topic).c_str());
  myClient.subscribe(String(mqtt2Topic).c_str());
  myClient.subscribe(String(mqtt_Setting).c_str());  
}

void connectWIFI(bool needDelay = true){
  WiFi.disconnect();
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_STA);
  WiFi.begin(_lwifi_ssid, _lwifi_pass);
  int impLoop = 0;
  if(needDelay){
    while (WiFi.status() != WL_CONNECTED && impLoop < maxLoop){ 
      delay(connectWIFIDelay); 
      impLoop++;
    }
  }
  delay(300);
}

void strToUint8(String str, uint8_t mqtt[][8],int rows,int cols) {
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      String subStr = str.substring((i * 8 + j) * 8, (i * 8 + j + 1) * 8);
      mqtt[i][j] = strtol(subStr.c_str(), NULL, 2);
    }
  }
  for (int k = 0; k < sizeof(mqtt) / sizeof(mqtt[0]); k++) {
  for (int l = 0; l < sizeof(mqtt[0]); l++) {
    Serial.print(mqtt[k][l]);
    Serial.print(" ");
  }
  Serial.println();
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length){
  receivedTopic=String(topic);
  receivedMsg="";
  for (unsigned int myIndex = 0; myIndex < length; myIndex++)
  {
    receivedMsg += (char)payload[myIndex];
  }
  receivedMsg.trim();
  Serial.println(receivedTopic);
  if(receivedTopic == mqtt1Topic){
    strToUint8(receivedMsg, mqtt1,16,8);
  }
  if(receivedTopic == mqtt2Topic){
    strToUint8(receivedMsg, mqtt2,16,8);
  }
  if(receivedTopic == mqtt_Setting){
    infraedFunc = receivedMsg.charAt(0) - '0';
    motorFunc = receivedMsg.charAt(1) - '0';;
  }
  if(!infraedFunc){ 
    initMax7219(mqtt1);
    lastInfraredMode = 0;
  }; // 紅外線功能沒有開啟，則預設顯示MQTT1
}

void initMax7219(byte mqtt[16][8]){
  for(int i=0;i<DEVICE_NUMBER;i++){
    for(int j=0;j<8;j++){
      myBitmap_max7219[j]=mqtt[i][j];
    }
    mx.setBuffer((DEVICE_NUMBER-i)*8-1, 8, myBitmap_max7219);  
  }
}


void setup()
{
  Serial.begin(115200);
  // 防彈跳按鈕的延遲
  resetBtn.setDebounceTime(50);
  // max7219初始化
  myDisplay.begin();
  myDisplay.displayClear();
  mx.begin();
  // HC-HR501
  pinMode(infraredPin, INPUT);
  digitalWrite(infraredPin, LOW);
  // WIFI提示的LED狀態
  pinMode(wifiNotice, OUTPUT);
  digitalWrite(wifiNotice, LOW);
  // MQTT提示的LED狀態
  pinMode(mqttNotice, OUTPUT);
  digitalWrite(mqttNotice, LOW);
  // WIFI連線
  connectWIFI();
  // MQTT
  myClient.setServer(MQTT_SERVER_IP, MQTT_SERVER_PORT);
  myClient.setCallback(mqttCallback);
  connectMQTT();
  dht11_p25.begin();
  // 宣告馬達驅動
  pinMode(motorPin, OUTPUT);
  digitalWrite(motorPin, LOW);
}

void loop()
{
  myClient.loop();
  resetBtn.loop();
  // 
  if(resetBtn.isPressed()) {
    Serial.println("按鈕被按下");
    if ((WiFi.status() != WL_CONNECTED)) {
      connectWIFI();
    }
    if(!myClient.connected()) {
      connectMQTT();
    }
  }

  if ((WiFi.status() != WL_CONNECTED)) {
    digitalWrite(wifiNotice, HIGH);
  } else {
    digitalWrite(wifiNotice, LOW);
  }
  if (!myClient.connected()) {
    digitalWrite(mqttNotice, HIGH);
  } else {
    digitalWrite(mqttNotice, LOW);
  }

  // 馬達驅動
  if(motorFunc){
    if (dht11_p25.readTemperature() >= balanceTempture && dht11_p25.readHumidity() >= balanceHumi) {
      // 馬達驅動
      digitalWrite(motorPin, HIGH);
    } else {
      // 馬達驅動
      digitalWrite(motorPin, LOW);
    }
  }else{
      digitalWrite(motorPin, LOW);
  }    
  // 預設有紅外線
  if(infraedFunc){ 
    // 紅外線沒偵測，高電位表示有人
    // 若沒人，顯示1
    // 若有人，顯示2
    if (!digitalRead(infraredPin) && lastInfraredMode!=0) {
      initMax7219(mqtt1);
      lastInfraredMode = 0;
    } else if(digitalRead(infraredPin) && lastInfraredMode!=1){
      initMax7219(mqtt2);
      lastInfraredMode = 1;
    }
  }else{
    // 若沒有紅外線則會顯示 第一模式
    lastInfraredMode = 0;
  }

  // max7219 重複顯示
  if (myDisplay.displayAnimate()) {myDisplay.displayReset();}

}
