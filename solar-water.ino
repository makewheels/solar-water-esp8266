#include <ESP8266WiFi.h>
/* 依赖 PubSubClient 2.4.0 */
#include <PubSubClient.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

//#define WIFI_SSID         "magic"
#define WIFI_SSID         "ChinaNet-RV8U7W"
#define WIFI_PASSWD       "qiaobinhao123"

#define MQTT_SERVER       "a1g0aXwTsx1.iot-as-mqtt.cn-shanghai.aliyuncs.com"
#define MQTT_PORT         1883
#define MQTT_USRNAME      "ZApjdvupCbBZlbJYFILD&a1g0aXwTsx1"
#define CLIENT_ID         "a1g0aXwTsx1.ZApjdvupCbBZlbJYFILD|securemode=2,signmethod=hmacsha256,timestamp=1658143431835|"
#define MQTT_PASSWD       "5ef55aa8f20a192c8fae1e55f9b0c4d0e127ad5310a4c138bdd9bbf0cd790ada"

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

//连接wifi
void connectWifi(){
    if (WiFi.status() == WL_CONNECTED){
        return;
    }
    Serial.println("Start connect WiFi...");
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWD);
    while (WiFi.status() != WL_CONNECTED){
        delay(1500);
        Serial.println("WiFi is Connecting");
    }

    Serial.print("Connected to AP, IP address: ");
    Serial.println(WiFi.localIP());
    connectMqtt();
}

//连接mqtt
void connectMqtt(){
    if(mqttClient.connected()){
        return;
    }
    mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
    mqttClient.setCallback(onReceiveMessage);
    while (!mqttClient.connected()){
        Serial.println("Connecting to MQTT Server ...");
        if (mqttClient.connect(CLIENT_ID, MQTT_USRNAME, MQTT_PASSWD)){
            Serial.println("MQTT Connected!");
        }else{
            Serial.print("MQTT Connect err: ");
            Serial.println(mqttClient.state());
            delay(3000);
        }
    }
}

//上水结束时间
unsigned long endTime = 0;
//当前状态
bool isOn = false;

//收到消息后的回调函数
void onReceiveMessage(char *topic, byte *payload, unsigned int length){
    payload[length] = '\0';
    int timeLength = String((char *)payload).toInt(); 
    
    pinMode(D1, OUTPUT);
    digitalWrite(D1, HIGH);
    endTime = millis() + timeLength;
    isOn = true;
}

void setup(){
    Serial.begin(115200);
    delay(200);
    connectWifi();
}

void loop(){
  //如果当前时间，已经超过了应该断开的时间。并且上水阀还是接通的状态，那就断开
  if(millis() > endTime && isOn){
    digitalWrite(D1, LOW);
    isOn = false;
  }
  connectWifi();
  mqttClient.loop();  //保持客户端的连接
  delay(500);
}
