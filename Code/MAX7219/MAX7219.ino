//Generated Date: Sat, 13 Jul 2024 09:26:40 GMT
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

ezButton resetBtn(2);
int btnDelay = 50;

float balanceTempture = 10;
float balanceHumi = 0;
uint8_t mqtt1[16][8] = {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0},{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0},{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}};
uint8_t mqtt2[16][8] = {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0},{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0},{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}};
char *max7219Char;
String max7219Str="";
MD_Parola myDisplay = MD_Parola(HARDWARE_TYPE,4,17,16,DEVICE_NUMBER);
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE,4,17,16,DEVICE_NUMBER);

 char _lwifi_ssid[] = "Gary";
 char _lwifi_pass[] = "0972728710";
//char _lwifi_ssid[] = "CMoney";
//char _lwifi_pass[] = "xyz8888888";
String receivedTopic="";
String receivedMsg="";
bool waitForE=true;
bool ended=true;
bool pubCtrl=false;

WiFiClient mqttClient;
PubSubClient myClient(mqttClient);

DHT dht11_p25(25, DHT11);
uint8_t myBitmap_max7219[8] ={0};
void connectMQTT(){
//  while (!myClient.connected()){
//    if (!myClient.connect(MQTT_ID,MQTT_USERNAME,MQTT_PASSWORD))
//    {
//      delay(5000);
//    }
//  }
myClient.connect(MQTT_ID,MQTT_USERNAME,MQTT_PASSWORD);
}

void strToUint8(String str, uint8_t mqtt[][8],int rows,int cols) {
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      String subStr = str.substring((i * 8 + j) * 8, (i * 8 + j + 1) * 8);
      mqtt[i][j] = strtol(subStr.c_str(), nullptr, 2);
    }
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
  if(receivedTopic == "max7219-mqtt1"){
    strToUint8(receivedMsg, mqtt1,16,8);
  }
  if(receivedTopic == "max7219-mqtt2"){
    strToUint8(receivedMsg, mqtt2,16,8);
  }
  initMax7219(mqtt1);
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
  resetBtn.setDebounceTime(50);

  myDisplay.begin();
  myDisplay.displayClear();
  mx.begin();
  pinMode(39, INPUT);

  pinMode(22, OUTPUT);
  pinMode(21, OUTPUT);
  // 是否能按按鈕
  // 上次按鈕電位狀態
  // 上次按鈕時間
  // 彈跳延遲時間
  //  HC-HR501
  digitalWrite(39, LOW);

  // 按鈕
  digitalWrite(22, LOW);
  // 按鈕
  digitalWrite(21, LOW);
  WiFi.disconnect();
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_STA);
  WiFi.begin(_lwifi_ssid, _lwifi_pass);
//  while (WiFi.status() != WL_CONNECTED) { delay(500); }
  delay(300);
  myClient.setServer(MQTT_SERVER_IP, MQTT_SERVER_PORT);
  myClient.setCallback(mqttCallback);
  connectMQTT();
  myClient.subscribe(String("max7219-mqtt1").c_str());
  myClient.subscribe(String("max7219-mqtt2").c_str());
  dht11_p25.begin();
  pinMode(33, OUTPUT);
}

void loop()
{

  resetBtn.loop();
  if (resetBtn.isPressed()) {
    Serial.println("按鈕被按下");
          if ((WiFi.status() != WL_CONNECTED)) {
        WiFi.disconnect();
        WiFi.softAPdisconnect(true);
        WiFi.mode(WIFI_STA);
        WiFi.begin(_lwifi_ssid, _lwifi_pass);
//        while (WiFi.status() != WL_CONNECTED) { delay(500); }
        delay(300);
      }
      if (!myClient.connected()) {
        connectMQTT();
      }
  }
  myClient.loop();
  if ((WiFi.status() != WL_CONNECTED)) {
    digitalWrite(22, HIGH);
  } else {
    digitalWrite(22, LOW);
  }
  if (!myClient.connected()) {
    digitalWrite(21, HIGH);
  } else {
    digitalWrite(21, LOW);
  }
  // 按下按鈕>比對時間>紀錄時間
  // 按鈕
//  Serial.println("Z");
//  Serial.println(digitalRead(2));
//    Serial.println(lastBtnState);
  
       
    // 按鈕

         
//      if ((WiFi.status() != WL_CONNECTED)) {
//        WiFi.disconnect();
//        WiFi.softAPdisconnect(true);
//        WiFi.mode(WIFI_STA);
//        WiFi.begin(_lwifi_ssid, _lwifi_pass);
//        while (WiFi.status() != WL_CONNECTED) { delay(500); }
//        delay(300);
//      }
//      if (!myClient.connected()) {
//        connectMQTT();
//      }


//  Serial.println(dht11_p25.readTemperature());
//    Serial.println(dht11_p25.readHumidity());
//  if (dht11_p25.readTemperature() >= balanceTempture && dht11_p25.readHumidity() >= balanceHumi) {
//    // 馬達驅動
//    digitalWrite(33, HIGH);
//  } else {
//    // 馬達驅動
//    digitalWrite(33, LOW);
//  }
  // 若沒人，顯示1
  // 若有人，顯示2
  //
  // 紅外線偵測，高電位表示有人
//  if (digitalRead(39)) {
//    initMax7219(mqtt1);
//  } else {
//    initMax7219(mqtt2);
//  }
  if (myDisplay.displayAnimate()) {myDisplay.displayReset();}

}
