/*
 * Blink
 * Turns on an LED on for one second,
 * then off for one second, repeatedly.
 */

#include <Arduino.h>

#ifndef LED_BUILTIN
  #define LED_BUILTIN PC13
#endif

#ifndef SPEED_CONTROLLER
  #define SPEED_CONTROLLER PA0
#endif

#ifndef DIRECTION_TOGGLE
  #define DIRECTION_TOGGLE PA10
#endif


#ifndef MOTORA_SPEED
  #define MOTORA_SPEED PB6
#endif

#ifndef IN1
  #define IN1 PB7
#endif

#ifndef IN2
  #define IN2 PB8
#endif

bool IN1VAL = HIGH;
bool IN2VAL = LOW;

void button_ISR()
{
  IN1VAL = !IN1VAL;
  IN2VAL = !IN2VAL;
}


void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(MOTORA_SPEED, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(SPEED_CONTROLLER, INPUT);
  pinMode(DIRECTION_TOGGLE, INPUT);
  attachInterrupt(digitalPinToInterrupt(DIRECTION_TOGGLE), button_ISR, CHANGE);
}

// the loop function runs over and over again forever
void loop() {
  int adcValue = analogRead(SPEED_CONTROLLER);
  int dutyCycle = map(adcValue, 0, 4095, 0, 65535);
  analogWrite(LED_BUILTIN, dutyCycle);  
  analogWrite(MOTORA_SPEED, dutyCycle);  
  digitalWrite(IN1, IN1VAL);
  digitalWrite(IN2, IN2VAL);
}

