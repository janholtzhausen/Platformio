#include <Arduino.h>
#include <ezTime.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#define EZTIME_CACHE_EEPROM

void switchOnLED () {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); 
}

void setup() {
	Serial.begin(9600);
	WiFi.begin("holtztech", "jan4loren50");

	waitForSync();

  setServer("nz.pool.ntp.org");
	setDebug(DEBUG);
  updateNTP();

	Serial.println("UTC: " + UTC.dateTime());
	
	Timezone NewZealand;
  if (!NewZealand.setCache(0)) NewZealand.setLocation("Pacific/Auckland");
	Serial.println("New Zealand time: " + NewZealand.dateTime());

  time_t ledtest = makeTime(23,43,0, day(), month(), year());

  setEvent(switchOnLED, ledtest);
}

void loop() {
  events();
}

