
#include <LiquidCrystal.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <PID_v1.h>

#include <Arduino.h>
#include <DNSServer.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);

const char* ssid = "TempControl-AP";

const char* input_parameterA = "input_A";
const char* input_parameterB = "input_B";

const int oneWireBus  = 15;
const int adcPin = 35;
const int aOutPin = 5;
const int bOutPin = 19;

const int freq = 5;
const int aChannel = 0;
const int bChannel = 1;
const int resolution = 8;


int adcValue = 0;
double aTarget = 30;
double bTarget = 80;
double aTemp = 0;
double bTemp = 0;

double aOutput;
double bOutput;

double supKp=16, supKi=0.8, supKd=4;
double aggKp=4, aggKi=0.2, aggKd=1;
double consKp=1, consKi=0.05, consKd=0.25;


PID aPID(&aTemp, &aOutput, &aTarget, consKp, consKi, consKd, DIRECT);
PID bPID(&bTemp, &bOutput, &bTarget, consKp, consKi, consKd, DIRECT);

OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);


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

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>HTML Form to Input Data</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html {font-family: Times New Roman; display: inline-block; text-align: center;}
    h2 {font-size: 3.0rem; color: #FF0000;}
  </style>
  </head><body>
  <h2>Set Target Temps</h2> 
  <h3>Enter New Values as numbers only</h3>
  <br>
  <form action="/get">
    Current Target A <b><mark>%PLACEHOLDER_A%:</mark></b> <input type="text" pattern="^\d*(\.\d{0,2})?$" name="input_A">
    <button type="submit">New<br />Target A</button>
  </form><br>
  <form action="/get">
    Current Target B <b><mark>%PLACEHOLDER_B%:</mark></b> <input type="text" pattern="^\d*(\.\d{0,2})?$" name="input_B">
    <button type="submit">New<br />Target B</button>
  </form>
</body></html>)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

String processor(const String& var)
{
 
  Serial.println(var);
 
  if(var == "PLACEHOLDER_A"){
    return String(aTarget);
  }
 
  else if(var == "PLACEHOLDER_B"){
    return String(bTarget);
  }
 
  return String();
}



void setup() {
  
  Serial.begin(9600);
  pinMode(aOutPin, OUTPUT);
  pinMode(bOutPin, OUTPUT);
  ledcAttachPin(aOutPin, aChannel);
  ledcAttachPin(bOutPin, bChannel);
  

  WiFi.softAP(ssid);
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor); 
  });

  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String input_A;
    String input_B;
    
    if (request->hasParam(input_parameterA)) {
      input_A = request->getParam(input_parameterA)->value();
      if (input_A) {
        aTarget = input_A.toDouble();
      }
    }
    else 
    if (request->hasParam(input_parameterB)) {
      input_B = request->getParam(input_parameterB)->value();
      if (input_B) {
        bTarget = input_B.toDouble();
      }
    }
    Serial.print("A = ");
    Serial.println(input_A);
    Serial.print("B = ");
    Serial.println(input_B);
    request->redirect("/");
  });
  server.onNotFound(notFound);
  server.begin();
  
  
  aPID.SetMode(AUTOMATIC);
  bPID.SetMode(AUTOMATIC);

  ledcSetup(aChannel, freq, resolution);
  ledcSetup(bChannel, freq, resolution);
  
  
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

void loop() {
  
  adcValue = analogRead(adcPin);
  sensors.requestTemperatures(); 
  aTemp = sensors.getTempCByIndex(0);
  bTemp = sensors.getTempCByIndex(1);
  lcd.setCursor(3, 2);
  lcd.print(aTemp);
  lcd.setCursor(3, 3);
  lcd.print(bTemp);

  
  lcd.setCursor(12, 2);
  lcd.print(aTarget);
  lcd.setCursor(12, 3);
  lcd.print(bTarget);
  
  if (adcValue < 2700 && adcValue > 2500 ) { // UP
    aTarget = aTarget + 0.25;
  } else 
  if (adcValue < 600) { // LEFT
    bTarget = bTarget + 0.25;
  } else
  if (adcValue < 1800 && adcValue > 1600 ) { // CENTER
    if (aTarget < 60) {
      aTarget = 60.00;
    } else
    if (aTarget == 60.00) {
      aTarget = 78.00;
    } else
    aTarget = 23.00;
  } else
  if (adcValue < 2300 && adcValue > 2100 ) { // RIGHT
    aTarget = aTarget - 0.25;
  } else
  if (adcValue < 1200 && adcValue > 1000 ) { // DOWN
    bTarget = bTarget - 0.25;
  } else {
    delay(1000);
  }

  double aGap = abs(aTarget-aTemp);
  if(aGap<2)
  {  
    aPID.SetTunings(consKp, consKi, consKd);
  }
  else
  if(aGap>=2 && aGap<10)
  {  
    aPID.SetTunings(aggKp, aggKi, aggKd);
  }
  else
  {
     aPID.SetTunings(supKp, supKi, supKd);
  }

  double bGap = abs(bTarget-bTemp);
  if(bGap<2)
  {  
    bPID.SetTunings(consKp, consKi, consKd);
  }
  else
  if(bGap>=2 && bGap<10)
  {  
    bPID.SetTunings(aggKp, aggKi, aggKd);
  }
  else
  {
     bPID.SetTunings(supKp, supKi, supKd);
  }
  
  aPID.Compute();
  Serial.print("A PID = ");
  Serial.println(aOutput);
  ledcWrite(aChannel, aOutput);
  
  bPID.Compute();
  Serial.print("B PID = ");
  Serial.println(bOutput);
  ledcWrite(bChannel, bOutput);
}
