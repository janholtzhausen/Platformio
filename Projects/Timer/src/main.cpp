#include <Arduino.h>
#include <StensTimer.h>

#define TICK_ACTION 1
#define TOCK_ACTION 5

StensTimer* stensTimer = NULL;
int ticker = 0;
int tocker = 0;

void timerCallback(Timer* timer){
  if (TICK_ACTION == timer->getAction()){
    Serial.println("tick: ");
  }
  if (TOCK_ACTION == timer->getAction()){
    Serial.println("tock: ");
  }
}
void setup() {
  Serial.begin(9600); 
  stensTimer = StensTimer::getInstance();
  stensTimer->setStaticCallback(timerCallback);
  Timer* myTickerTimer = stensTimer->setInterval(TICK_ACTION, 1000);
  Timer* myTockerTimer = stensTimer->setInterval(TOCK_ACTION, 10000);
}

void loop() {
  stensTimer->run();
}