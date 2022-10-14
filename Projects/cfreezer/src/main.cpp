#include <parameters.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <AsyncMqttClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ezTime.h>


#define RELAYPIN D1
#define ONE_WIRE_BUS D4

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);

AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
Ticker wifiReconnectTimer;

void connectToWifi() {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void connectToMqtt() {
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}

void onWifiConnect(const WiFiEventStationModeGotIP& event) {
  Serial.println("Connected to Wi-Fi.");
  connectToMqtt();
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected& event) {
  Serial.println("Disconnected from Wi-Fi.");
  mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
  wifiReconnectTimer.once(2, connectToWifi);
}

void onMqttConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);
  uint16_t packetIdSub = mqttClient.subscribe(MQTT_SUB, 2);
  Serial.print("Subscribing to ");
  Serial.println(MQTT_SUB);
  Serial.print("packetId: ");
  Serial.println(packetIdSub);
  uint16_t packetIdPub = mqttClient.publish(MQTT_PUB, 2, true, "test 2");
  Serial.print("Publishing to ");
  Serial.println(MQTT_PUB);
  Serial.print("packetId: ");
  Serial.println(packetIdPub);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Disconnected from MQTT.");

  if (WiFi.isConnected()) {
    mqttReconnectTimer.once(2, connectToMqtt);
  }
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
  Serial.println("Subscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
  Serial.print("  qos: ");
  Serial.println(qos);
}

void onMqttUnsubscribe(uint16_t packetId) {
  Serial.println("Unsubscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {

  Serial.println("Publish received.");
  Serial.print("  topic: ");
  Serial.println(topic);
  Serial.print("  payload: ");
  Serial.println(payload);
  if ((char)payload[0] == '1') {
    Serial.println("on");
    digitalWrite(RELAYPIN, HIGH);
  } else {
    Serial.println("off");
    digitalWrite(RELAYPIN, LOW);
  }
  Serial.print("  qos: ");
  Serial.println(properties.qos);
}

void onMqttPublish(uint16_t packetId) {
//  Serial.println("Publish acknowledged.");
//  Serial.print("  packetId: ");
//  Serial.println(packetId);
}

void setup() {
  Serial.begin(115200);
  DS18B20.begin();
  pinMode(RELAYPIN, OUTPUT);

  wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
  wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);

  connectToWifi();
  setServer("nz.pool.ntp.org");      
  waitForSync(60);
  Serial.println("UTC: " + UTC.dateTime());
  Timezone NewZealand;
	NewZealand.setLocation("Pacific/Auckland");
	Serial.println("New Zealand time: " + NewZealand.dateTime());
}

void loop() {
  updateNTP();
  DS18B20.requestTemperatures();
  float temp = DS18B20.getTempCByIndex(0);
  Timezone NewZealand;
	NewZealand.setLocation("Pacific/Auckland");
  if ((NewZealand.dateTime("H") == "21")) {
    digitalWrite(RELAYPIN, HIGH);
    Serial.print("Hour of power, 21:00-22:00. Freezer ON");
    mqttClient.publish(MQTT_PUB, 2, true, "Hour of power. Freezer ON");
  } else {
    if (temp > 22 ) {
      digitalWrite(RELAYPIN, HIGH);
    } else {
      digitalWrite(RELAYPIN, LOW);
    }
  }
  Serial.println(String(temp).c_str());
  mqttClient.publish(MQTT_PUB, 2, true, String(temp).c_str());
  delay(60000); 
}