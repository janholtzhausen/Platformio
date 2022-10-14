#include <Arduino.h>
#include <ezTime.h>
#include <WiFi.h>
#include <DHT.h>
#include <PID_v1.h>
#include <ESPAsyncWebServer.h>


const double KP = 2.00; 
const double KI = 1.00; 
const double KD = 1.00; 

double HD = 0;                       
double HUMIDITY_SETPOINT = 12;         

// Replace with your network credentials
const char* ssid = "holtztech";
const char* password = "jan4loren50";

#define DHTPIN 16     // Digital pin connected to the DHT sensor
#define HEAPIN 22

const int freq = 50;
const int pwmChannel = 0;
const int resolution = 12;
double dutyCycle = 4095.00;
double dutyCyclePercent = 100.00;

// Uncomment the type of sensor in use:
//#define DHTTYPE    DHT11     // DHT 11
#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);

// current temperature & humidity, updated in loop()
double t = 0.0;
double h = 0.0;

PID HEATER_PID(&HD, &dutyCycle, &HUMIDITY_SETPOINT, KP, KI, KD, REVERSE); 

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;    // will store last time DHT was updated

// Updates DHT readings every 3 seconds
const long interval = 3000;  

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h2>Holtztech</h2>
  <h3>Environmental Control Cabinet</h3>
  <p>
    <i class="fas fa-stopwatch" style="color:#9e0519;"></i> 
    <span class="dht-labels">Duty Cycle</span> 
    <span id="dutycycle">%dutyCyclePercent%</span>
    <sup class="units">%</sup>
  </p>
  <p>
    <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
    <span class="dht-labels">Temperature</span> 
    <span id="temperature">%TEMPERATURE%</span>
    <sup class="units">&deg;C</sup>
  </p>
  <p>
    <i class="fas fa-tint" style="color:#00add6;"></i> 
    <span class="dht-labels">Humidity</span>
    <span id="humidity">%HUMIDITY%</span>
    <sup class="units">%</sup>
  </p>
</body>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("dutyCyclePercent").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/dutyCyclePercent", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperature").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperature", true);
  xhttp.send();
}, 10000 ) ;


setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("humidity").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/humidity", true);
  xhttp.send();
}, 10000 ) ;
</script>
</html>)rawliteral";

// Replaces placeholder with DHT values
String processor(const String& var){
  //Serial.println(var);
  if(var == "dutyCyclePercent"){
    return String(dutyCyclePercent);
  }
  else if(var == "TEMPERATURE"){
    return String(t);
  }
  else if(var == "HUMIDITY"){
    return String(h);
  }
  return String();
}

void setup(){
  ledcSetup(pwmChannel, freq, resolution);
  ledcAttachPin(HEAPIN, pwmChannel);
  dht.begin();
  HEATER_PID.SetMode(AUTOMATIC); 
  HEATER_PID.SetOutputLimits(0,4095); 
  
  // Serial port for debugging purposes
  Serial.begin(115200);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(".");
  }

  Serial.println(WiFi.localIP());
  waitForSync();
  
    

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/dutyCyclePercent", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(dutyCyclePercent).c_str());
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(t).c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(h).c_str());
  });

  // Start server
  server.begin();
}
 
void loop(){   
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you updated the DHT values
    previousMillis = currentMillis;
    // Read temperature as Celsius (the default)
    double newT = dht.readTemperature();
    if (isnan(newT)) {
      Serial.println("Failed to read from DHT sensor!");
      digitalWrite(HEAPIN, LOW); 
    }
    else {
      t = newT;
    }
    // Read Humidity
    double newH = dht.readHumidity();
    // if humidity read failed, don't change h value 
    if (isnan(newH)) {
      Serial.println("Failed to read from DHT sensor!");
    }
    else {
        h = newH;
        HD = newH;
        HEATER_PID.Compute();
        ledcWrite(pwmChannel, dutyCycle);
        dutyCyclePercent = ((dutyCycle/4095)*100);
        Serial.println("-----------------------------------------------");
        Timezone NewZealand;
        NewZealand.setLocation("Pacific/Auckland");
        Serial.println(NewZealand.dateTime());
        Serial.print("Duty Cycle Percent = ");
        Serial.println(dutyCyclePercent);
        Serial.print("Box Temperature = ");
        Serial.println(t);
        Serial.print("Box Humidity = ");
        Serial.println(h);
        Serial.println("-----------------------------------------------");
        Serial.println("");
    }
  }
}