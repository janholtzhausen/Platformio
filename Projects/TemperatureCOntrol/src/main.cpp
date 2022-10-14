#include <Arduino.h>
#include <LiquidCrystal.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <max6675.h>


int thermoDO = 19;
int thermoCS = 5;
int thermoCLK = 18;


const int oneWireBus  = 15;
const int adcPin = 35;
int adcValue = 0;
float aSetPoint = 0;
float bSetPoint = 0;
float aTarget = 0;
float bTarget = 0;



OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);
MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);


LiquidCrystal lcd(14, 27, 26, 25, 33, 32);

uint8_t degsym[8] = {
  0b00110,
  0b01001,
  0b01001,
  0b00110,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
};

uint8_t arrow[8] = {
  0b00000,
  0b00100,
  0b00110,
  0b11111,
  0b00110,
  0b00100,
  0b00000,
  0b00000
};

void setup() {
  Serial.begin(115200);
  lcd.begin(20, 4);
  lcd.createChar(1, degsym);
  lcd.createChar(2, arrow);
  lcd.setCursor(0, 0);
  lcd.write("Temperature Control");
  lcd.setCursor(2, 1);
  lcd.write ("SENSOR");
  lcd.setCursor(11, 1);
  lcd.write ("TARGET");
  lcd.setCursor(0, 2);
  lcd.write ("A");
  lcd.setCursor(9, 2);
  lcd.write ("\x02");
  lcd.setCursor(0, 3);
  lcd.write ("B");
  lcd.setCursor(9, 3);
  lcd.write ("\x02");
  lcd.setCursor(18, 2);
  lcd.write ("C\x01");
  lcd.setCursor(18, 3);
  lcd.write ("C\x01");
  sensors.begin();
  
}

void incrimentA () {
    aSetPoint = aSetPoint + 0.1;
    lcd.setCursor(9, 2);
    lcd.write ("?");
}

void loop() {
  adcValue = analogRead(adcPin);
  sensors.requestTemperatures(); 
  float temperatureC = sensors.getTempCByIndex(0);
  float temp = (thermocouple.readCelsius());
  lcd.setCursor(3, 2);
  lcd.print(temperatureC);
  lcd.setCursor(3, 3);
  lcd.print(temp);

  
  lcd.setCursor(12, 2);
  lcd.print(aSetPoint);
  lcd.setCursor(12, 3);
  lcd.print(bSetPoint);

  if (adcValue < 2700 && adcValue > 2500 ) { // UP
    incrimentA ();
  } else 
  if (adcValue < 600) { // LEFT
    bSetPoint = bSetPoint + 0.25;
    lcd.setCursor(9, 3);
    lcd.write ("?");
  } else
  if (adcValue < 1800 && adcValue > 1600 ) { // OK
    lcd.setCursor(9, 2);
    lcd.write ("\x02");
    lcd.setCursor(9, 3);
    lcd.write ("\x02");
  } else
  if (adcValue < 2300 && adcValue > 2100 ) { // RIGHT
    aSetPoint = aSetPoint - 0.1;
    lcd.setCursor(9, 2);
    lcd.write ("?");
  } else
  if (adcValue < 1200 && adcValue > 1000 ) { // DOWN
    bSetPoint = bSetPoint - 0.25;
    lcd.setCursor(9, 3);
    lcd.write ("?");
  } else {
    delay(1000);
  }
     
}