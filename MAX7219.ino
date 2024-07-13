//Generated Date: Sat, 13 Jul 2024 10:37:10 GMT

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

boolean canClickBtn = false;
int lastBtnState = false;
long lastClickTime = 0;
long btnDelay = 50;
float balanceTempture = 25;
float balanceHumi = 50;
byte mqtt1[3] = "";
byte mqtt2[16] = "";
char *max7219Char;
String max7219Str="";
MD_Parola myDisplay = MD_Parola(HARDWARE_TYPE,4,17,16,DEVICE_NUMBER);
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE,4,17,16,DEVICE_NUMBER);

char _lwifi_ssid[] = "Gary";
char _lwifi_pass[] = "0972728710";
String receivedTopic="";
String receivedMsg="";
bool waitForE=true;
bool ended=true;
bool pubCtrl=false;

WiFiClient mqttClient;
PubSubClient myClient(mqttClient);

DHT dht11_p36(36, DHT11);
uint8_t myBitmap_max7219[8] ={0};
void connectMQTT(){
  while (!myClient.connected()){
    if (!myClient.connect(MQTT_ID,MQTT_USERNAME,MQTT_PASSWORD))
    {
      delay(5000);
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
  Serial.println(receivedMsg);

}

void setup()
{
  Serial.begin(9600);

  myDisplay.begin();
  myDisplay.displayClear();
  mx.begin();
  pinMode(39, INPUT);
  pinMode(2, INPUT);
  pinMode(22, OUTPUT);
  pinMode(21, OUTPUT);
  // 是否能按按鈕
  // 上次按鈕電位狀態
  // 上次按鈕時間
  // 彈跳延遲時間
  //  HC-HR501
  digitalWrite(39, LOW);
  // 按鈕
  digitalWrite(2, LOW);
  // 按鈕
  digitalWrite(22, LOW);
  // 按鈕
  digitalWrite(21, LOW);
  WiFi.disconnect();
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_STA);
  WiFi.begin(_lwifi_ssid, _lwifi_pass);
  while (WiFi.status() != WL_CONNECTED) { delay(500); }
  delay(300);
  myClient.setServer(MQTT_SERVER_IP, MQTT_SERVER_PORT);
  myClient.setCallback(mqttCallback);
  connectMQTT();
  myClient.subscribe(String("max7219-mqtt1").c_str());
  myClient.subscribe(String("max7219-mqtt2").c_str());
  dht11_p36.begin();
  pinMode(33, OUTPUT);
}

void loop()
{
  myClient.loop();
  if ((WiFi.status() != WL_CONNECTED)) {
    // 按鈕
    digitalWrite(22, HIGH);
  } else {
    // 按鈕
    digitalWrite(22, LOW);
  }
  if (!myClient.connected()) {
    // 按鈕
    digitalWrite(21, HIGH);
  } else {
    // 按鈕
    digitalWrite(21, LOW);
  }
  // 按下按鈕>比對時間>紀錄時間
  // 按鈕
  if (digitalRead(2) != lastBtnState) {
    lastClickTime = millis();
  }
  // 按下按鈕>比對時間>紀錄時間
  if (millis() - lastClickTime > btnDelay) {
    // 按鈕
    // 按鈕
    if (canClickBtn == false && digitalRead(2)) {
      if ((WiFi.status() != WL_CONNECTED)) {
        WiFi.disconnect();
        WiFi.softAPdisconnect(true);
        WiFi.mode(WIFI_STA);
        WiFi.begin(_lwifi_ssid, _lwifi_pass);
        while (WiFi.status() != WL_CONNECTED) { delay(500); }
        delay(300);
      }
      if (!myClient.connected()) {
        connectMQTT();
      }
      canClickBtn = true;
    } else if (canClickBtn == true && !digitalRead(2)) {
      canClickBtn = false;
    }
  }
  // 按鈕
  lastBtnState = digitalRead(2);
  if (dht11_p36.readTemperature() >= balanceTempture && dht11_p36.readHumidity() >= balanceHumi) {
    // 馬達驅動
    digitalWrite(33, HIGH);
  } else {
    // 馬達驅動
    digitalWrite(33, LOW);
  }
  for(int i=0;i<8;i++){
    myBitmap_max7219[0]=B11111111;
    myBitmap_max7219[1]=B00000000;
    myBitmap_max7219[2]=B00000000;
    myBitmap_max7219[3]=B00000000;
    myBitmap_max7219[4]=B00000000;
    myBitmap_max7219[5]=B00000000;
    myBitmap_max7219[6]=B00000000;
    myBitmap_max7219[7]=B00000000;
  }
  mx.setBuffer((DEVICE_NUMBER-0)*8-1, 8, myBitmap_max7219);
  for(int i=0;i<8;i++){
    myBitmap_max7219[0]=B00000000;
    myBitmap_max7219[1]=B11111111;
    myBitmap_max7219[2]=B00000000;
    myBitmap_max7219[3]=B00000000;
    myBitmap_max7219[4]=B00000000;
    myBitmap_max7219[5]=B00000000;
    myBitmap_max7219[6]=B00000000;
    myBitmap_max7219[7]=B00000000;
  }
  mx.setBuffer((DEVICE_NUMBER-1)*8-1, 8, myBitmap_max7219);
  for(int i=0;i<8;i++){
    myBitmap_max7219[0]=B00000000;
    myBitmap_max7219[1]=B00000000;
    myBitmap_max7219[2]=B11111111;
    myBitmap_max7219[3]=B00000000;
    myBitmap_max7219[4]=B00000000;
    myBitmap_max7219[5]=B00000000;
    myBitmap_max7219[6]=B00000000;
    myBitmap_max7219[7]=B00000000;
  }
  mx.setBuffer((DEVICE_NUMBER-2)*8-1, 8, myBitmap_max7219);
  for(int i=0;i<8;i++){
    myBitmap_max7219[0]=B00000000;
    myBitmap_max7219[1]=B00000000;
    myBitmap_max7219[2]=B00000000;
    myBitmap_max7219[3]=B11111111;
    myBitmap_max7219[4]=B00000000;
    myBitmap_max7219[5]=B00000000;
    myBitmap_max7219[6]=B00000000;
    myBitmap_max7219[7]=B00000000;
  }
  mx.setBuffer((DEVICE_NUMBER-3)*8-1, 8, myBitmap_max7219);
  for(int i=0;i<8;i++){
    myBitmap_max7219[0]=B00000000;
    myBitmap_max7219[1]=B00000000;
    myBitmap_max7219[2]=B00000000;
    myBitmap_max7219[3]=B00000000;
    myBitmap_max7219[4]=B11111111;
    myBitmap_max7219[5]=B00000000;
    myBitmap_max7219[6]=B00000000;
    myBitmap_max7219[7]=B00000000;
  }
  mx.setBuffer((DEVICE_NUMBER-4)*8-1, 8, myBitmap_max7219);
  for(int i=0;i<8;i++){
    myBitmap_max7219[0]=B00000000;
    myBitmap_max7219[1]=B00000000;
    myBitmap_max7219[2]=B00000000;
    myBitmap_max7219[3]=B00000000;
    myBitmap_max7219[4]=B00000000;
    myBitmap_max7219[5]=B11111111;
    myBitmap_max7219[6]=B00000000;
    myBitmap_max7219[7]=B00000000;
  }
  mx.setBuffer((DEVICE_NUMBER-5)*8-1, 8, myBitmap_max7219);
  for(int i=0;i<8;i++){
    myBitmap_max7219[0]=B00000000;
    myBitmap_max7219[1]=B00000000;
    myBitmap_max7219[2]=B00000000;
    myBitmap_max7219[3]=B00000000;
    myBitmap_max7219[4]=B00000000;
    myBitmap_max7219[5]=B00000000;
    myBitmap_max7219[6]=B11111111;
    myBitmap_max7219[7]=B00000000;
  }
  mx.setBuffer((DEVICE_NUMBER-6)*8-1, 8, myBitmap_max7219);
  for(int i=0;i<8;i++){
    myBitmap_max7219[0]=B00000000;
    myBitmap_max7219[1]=B00000000;
    myBitmap_max7219[2]=B00000000;
    myBitmap_max7219[3]=B00000000;
    myBitmap_max7219[4]=B00000000;
    myBitmap_max7219[5]=B00000000;
    myBitmap_max7219[6]=B00000000;
    myBitmap_max7219[7]=B11111111;
  }
  mx.setBuffer((DEVICE_NUMBER-7)*8-1, 8, myBitmap_max7219);
  for(int i=0;i<8;i++){
    myBitmap_max7219[0]=B11111111;
    myBitmap_max7219[1]=B00000001;
    myBitmap_max7219[2]=B00000001;
    myBitmap_max7219[3]=B00000001;
    myBitmap_max7219[4]=B00000001;
    myBitmap_max7219[5]=B00000001;
    myBitmap_max7219[6]=B00000001;
    myBitmap_max7219[7]=B00000001;
  }
  mx.setBuffer((DEVICE_NUMBER-8)*8-1, 8, myBitmap_max7219);
  for(int i=0;i<8;i++){
    myBitmap_max7219[0]=B00000010;
    myBitmap_max7219[1]=B11111111;
    myBitmap_max7219[2]=B00000010;
    myBitmap_max7219[3]=B00000010;
    myBitmap_max7219[4]=B00000010;
    myBitmap_max7219[5]=B00000010;
    myBitmap_max7219[6]=B00000010;
    myBitmap_max7219[7]=B00000010;
  }
  mx.setBuffer((DEVICE_NUMBER-9)*8-1, 8, myBitmap_max7219);
  for(int i=0;i<8;i++){
    myBitmap_max7219[0]=B00001000;
    myBitmap_max7219[1]=B00001000;
    myBitmap_max7219[2]=B00001000;
    myBitmap_max7219[3]=B11111111;
    myBitmap_max7219[4]=B00001000;
    myBitmap_max7219[5]=B00001000;
    myBitmap_max7219[6]=B00001000;
    myBitmap_max7219[7]=B00001000;
  }
  mx.setBuffer((DEVICE_NUMBER-10)*8-1, 8, myBitmap_max7219);
  for(int i=0;i<8;i++){
    myBitmap_max7219[0]=B00010000;
    myBitmap_max7219[1]=B00010000;
    myBitmap_max7219[2]=B00010000;
    myBitmap_max7219[3]=B00010000;
    myBitmap_max7219[4]=B11111111;
    myBitmap_max7219[5]=B00010000;
    myBitmap_max7219[6]=B00010000;
    myBitmap_max7219[7]=B00010000;
  }
  mx.setBuffer((DEVICE_NUMBER-11)*8-1, 8, myBitmap_max7219);
  for(int i=0;i<8;i++){
    myBitmap_max7219[0]=B00100000;
    myBitmap_max7219[1]=B00100000;
    myBitmap_max7219[2]=B00100000;
    myBitmap_max7219[3]=B00100000;
    myBitmap_max7219[4]=B00100000;
    myBitmap_max7219[5]=B11111111;
    myBitmap_max7219[6]=B00100000;
    myBitmap_max7219[7]=B00100000;
  }
  mx.setBuffer((DEVICE_NUMBER-12)*8-1, 8, myBitmap_max7219);
  for(int i=0;i<8;i++){
    myBitmap_max7219[0]=B01000000;
    myBitmap_max7219[1]=B01000000;
    myBitmap_max7219[2]=B01000000;
    myBitmap_max7219[3]=B01000000;
    myBitmap_max7219[4]=B01000000;
    myBitmap_max7219[5]=B01000000;
    myBitmap_max7219[6]=B11111111;
    myBitmap_max7219[7]=B01000000;
  }
  mx.setBuffer((DEVICE_NUMBER-13)*8-1, 8, myBitmap_max7219);
  for(int i=0;i<8;i++){
    myBitmap_max7219[0]=B10000000;
    myBitmap_max7219[1]=B10000000;
    myBitmap_max7219[2]=B10000000;
    myBitmap_max7219[3]=B10000000;
    myBitmap_max7219[4]=B10000000;
    myBitmap_max7219[5]=B10000000;
    myBitmap_max7219[6]=B10000000;
    myBitmap_max7219[7]=B11111111;
  }
  mx.setBuffer((DEVICE_NUMBER-14)*8-1, 8, myBitmap_max7219);
  for(int i=0;i<8;i++){
    myBitmap_max7219[0]=B00000100;
    myBitmap_max7219[1]=B00000100;
    myBitmap_max7219[2]=B11111111;
    myBitmap_max7219[3]=B00000100;
    myBitmap_max7219[4]=B00000100;
    myBitmap_max7219[5]=B00000100;
    myBitmap_max7219[6]=B00000100;
    myBitmap_max7219[7]=B00000100;
  }
  mx.setBuffer((DEVICE_NUMBER-15)*8-1, 8, myBitmap_max7219);
"CMoney";

"xyz88888888";

// 若沒人，顯示1
// 若有人，顯示2
//
//
//
//
//
//
//
// 紅外線偵測，高電位表示有人
if (digitalRead(39)) {
} else {
  for(int i=0;i<8;i++){
    myBitmap_max7219[0]=B11111111;
    myBitmap_max7219[1]=B00000000;
    myBitmap_max7219[2]=B00000000;
    myBitmap_max7219[3]=B00000000;
    myBitmap_max7219[4]=B00000000;
    myBitmap_max7219[5]=B00000000;
    myBitmap_max7219[6]=B00000000;
    myBitmap_max7219[7]=B00000000;
  }
  mx.setBuffer((DEVICE_NUMBER-0)*8-1, 8, myBitmap_max7219);
  for(int i=0;i<8;i++){
    myBitmap_max7219[0]=B00000000;
    myBitmap_max7219[1]=B11111111;
    myBitmap_max7219[2]=B00000000;
    myBitmap_max7219[3]=B00000000;
    myBitmap_max7219[4]=B00000000;
    myBitmap_max7219[5]=B00000000;
    myBitmap_max7219[6]=B00000000;
    myBitmap_max7219[7]=B00000000;
  }
  mx.setBuffer((DEVICE_NUMBER-1)*8-1, 8, myBitmap_max7219);
  for(int i=0;i<8;i++){
    myBitmap_max7219[0]=B00000000;
    myBitmap_max7219[1]=B00000000;
    myBitmap_max7219[2]=B11111111;
    myBitmap_max7219[3]=B00000000;
    myBitmap_max7219[4]=B00000000;
    myBitmap_max7219[5]=B00000000;
    myBitmap_max7219[6]=B00000000;
    myBitmap_max7219[7]=B00000000;
  }
  mx.setBuffer((DEVICE_NUMBER-2)*8-1, 8, myBitmap_max7219);
  for(int i=0;i<8;i++){
    myBitmap_max7219[0]=B00000000;
    myBitmap_max7219[1]=B00000000;
    myBitmap_max7219[2]=B00000000;
    myBitmap_max7219[3]=B11111111;
    myBitmap_max7219[4]=B00000000;
    myBitmap_max7219[5]=B00000000;
    myBitmap_max7219[6]=B00000000;
    myBitmap_max7219[7]=B00000000;
  }
  mx.setBuffer((DEVICE_NUMBER-3)*8-1, 8, myBitmap_max7219);
  for(int i=0;i<8;i++){
    myBitmap_max7219[0]=B00000000;
    myBitmap_max7219[1]=B00000000;
    myBitmap_max7219[2]=B00000000;
    myBitmap_max7219[3]=B00000000;
    myBitmap_max7219[4]=B11111111;
    myBitmap_max7219[5]=B00000000;
    myBitmap_max7219[6]=B00000000;
    myBitmap_max7219[7]=B00000000;
  }
  mx.setBuffer((DEVICE_NUMBER-4)*8-1, 8, myBitmap_max7219);
  for(int i=0;i<8;i++){
    myBitmap_max7219[0]=B00000000;
    myBitmap_max7219[1]=B00000000;
    myBitmap_max7219[2]=B00000000;
    myBitmap_max7219[3]=B00000000;
    myBitmap_max7219[4]=B00000000;
    myBitmap_max7219[5]=B11111111;
    myBitmap_max7219[6]=B00000000;
    myBitmap_max7219[7]=B00000000;
  }
  mx.setBuffer((DEVICE_NUMBER-5)*8-1, 8, myBitmap_max7219);
  for(int i=0;i<8;i++){
    myBitmap_max7219[0]=B00000000;
    myBitmap_max7219[1]=B00000000;
    myBitmap_max7219[2]=B00000000;
    myBitmap_max7219[3]=B00000000;
    myBitmap_max7219[4]=B00000000;
    myBitmap_max7219[5]=B00000000;
    myBitmap_max7219[6]=B11111111;
    myBitmap_max7219[7]=B00000000;
  }
  mx.setBuffer((DEVICE_NUMBER-6)*8-1, 8, myBitmap_max7219);
  for(int i=0;i<8;i++){
    myBitmap_max7219[0]=B00000000;
    myBitmap_max7219[1]=B00000000;
    myBitmap_max7219[2]=B00000000;
    myBitmap_max7219[3]=B00000000;
    myBitmap_max7219[4]=B00000000;
    myBitmap_max7219[5]=B00000000;
    myBitmap_max7219[6]=B00000000;
    myBitmap_max7219[7]=B11111111;
  }
  mx.setBuffer((DEVICE_NUMBER-7)*8-1, 8, myBitmap_max7219);
  for(int i=0;i<8;i++){
    myBitmap_max7219[0]=B11111111;
    myBitmap_max7219[1]=B00000000;
    myBitmap_max7219[2]=B00000000;
    myBitmap_max7219[3]=B00000000;
    myBitmap_max7219[4]=B00000000;
    myBitmap_max7219[5]=B00000000;
    myBitmap_max7219[6]=B00000000;
    myBitmap_max7219[7]=B00000000;
  }
  mx.setBuffer((DEVICE_NUMBER-8)*8-1, 8, myBitmap_max7219);
  for(int i=0;i<8;i++){
    myBitmap_max7219[0]=B00000000;
    myBitmap_max7219[1]=B11111111;
    myBitmap_max7219[2]=B00000000;
    myBitmap_max7219[3]=B00000000;
    myBitmap_max7219[4]=B00000000;
    myBitmap_max7219[5]=B00000000;
    myBitmap_max7219[6]=B00000000;
    myBitmap_max7219[7]=B00000000;
  }
  mx.setBuffer((DEVICE_NUMBER-9)*8-1, 8, myBitmap_max7219);
  for(int i=0;i<8;i++){
    myBitmap_max7219[0]=B00000000;
    myBitmap_max7219[1]=B00000000;
    myBitmap_max7219[2]=B11111111;
    myBitmap_max7219[3]=B00000000;
    myBitmap_max7219[4]=B00000000;
    myBitmap_max7219[5]=B00000000;
    myBitmap_max7219[6]=B00000000;
    myBitmap_max7219[7]=B00000000;
  }
  mx.setBuffer((DEVICE_NUMBER-10)*8-1, 8, myBitmap_max7219);
  for(int i=0;i<8;i++){
    myBitmap_max7219[0]=B00000000;
    myBitmap_max7219[1]=B00000000;
    myBitmap_max7219[2]=B00000000;
    myBitmap_max7219[3]=B11111111;
    myBitmap_max7219[4]=B00000000;
    myBitmap_max7219[5]=B00000000;
    myBitmap_max7219[6]=B00000000;
    myBitmap_max7219[7]=B00000000;
  }
  mx.setBuffer((DEVICE_NUMBER-11)*8-1, 8, myBitmap_max7219);
  for(int i=0;i<8;i++){
    myBitmap_max7219[0]=B00000000;
    myBitmap_max7219[1]=B00000000;
    myBitmap_max7219[2]=B00000000;
    myBitmap_max7219[3]=B00000000;
    myBitmap_max7219[4]=B11111111;
    myBitmap_max7219[5]=B00000000;
    myBitmap_max7219[6]=B00000000;
    myBitmap_max7219[7]=B00000000;
  }
  mx.setBuffer((DEVICE_NUMBER-12)*8-1, 8, myBitmap_max7219);
  for(int i=0;i<8;i++){
    myBitmap_max7219[0]=B00000000;
    myBitmap_max7219[1]=B00000000;
    myBitmap_max7219[2]=B00000000;
    myBitmap_max7219[3]=B00000000;
    myBitmap_max7219[4]=B00000000;
    myBitmap_max7219[5]=B11111111;
    myBitmap_max7219[6]=B00000000;
    myBitmap_max7219[7]=B00000000;
  }
  mx.setBuffer((DEVICE_NUMBER-13)*8-1, 8, myBitmap_max7219);
  for(int i=0;i<8;i++){
    myBitmap_max7219[0]=B00000000;
    myBitmap_max7219[1]=B00000000;
    myBitmap_max7219[2]=B00000000;
    myBitmap_max7219[3]=B00000000;
    myBitmap_max7219[4]=B00000000;
    myBitmap_max7219[5]=B00000000;
    myBitmap_max7219[6]=B11111111;
    myBitmap_max7219[7]=B00000000;
  }
  mx.setBuffer((DEVICE_NUMBER-14)*8-1, 8, myBitmap_max7219);
  for(int i=0;i<8;i++){
    myBitmap_max7219[0]=B00000000;
    myBitmap_max7219[1]=B00000000;
    myBitmap_max7219[2]=B00000000;
    myBitmap_max7219[3]=B00000000;
    myBitmap_max7219[4]=B00000000;
    myBitmap_max7219[5]=B00000000;
    myBitmap_max7219[6]=B00000000;
    myBitmap_max7219[7]=B11111111;
  }
  mx.setBuffer((DEVICE_NUMBER-15)*8-1, 8, myBitmap_max7219);
}

}
