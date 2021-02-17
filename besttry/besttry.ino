#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);

//////////////////////////////////////////////////////////////////
unsigned long interval = 10000;
unsigned long previousMillis = 0;
unsigned long interval1 = 1000;
unsigned long previousMillis1 = 0;


///////////////////////////////////////////////////////////////
//soil moisture pin port on nodmcu
const int moisturePin = A0;
//motor pin that connect the actuater  to relay on nodmcu 
const int motorPin = D0;

#define DHTTYPE DHT11 
// DHT Sensor

#define DHTPIN D6
uint8_t DHTPin = D6; 
#define DHTTYPE DHT11

DHT dht(DHTPIN,DHTTYPE);
float Temperature;
float Humidity;
float moisturePercentage; 


// Replace with your network credentials
const char* ssid     = "try1";
const char* password = "12345678";

// REPLACE with your Domain name and URL path or IP address with path
const char* serverName = "http://192.168.43.129/post-esp-data.php";

// Keep this API Key value to be compatible with the PHP code provided in the project page. 
// If you change the apiKeyValue value, the PHP file /post-esp-data.php also needs to have the same key 
String apiKeyValue = "tPmAT5Ab3j7F9";

String sensorName = "Sensor12";
String sensorLocation = "Dilla";



void setup() {

  pinMode(DHTPin, INPUT);
  pinMode(motorPin, OUTPUT);
  digitalWrite(motorPin, LOW); 

  dht.begin(); 
  lcd.clear();             
  lcd.begin(16,2);
  lcd.init();
  lcd.backlight();
  
  lcd.setCursor(0,0);
  lcd.print("Powered By:");
  lcd.setCursor(0,1);
  lcd.print("Smart Farming");
  delay(4000);
  lcd.clear(); 

 lcd.clear(); 
 lcd.setCursor(0,0);
 lcd.print("Project By:");
 lcd.setCursor(0,1);
 lcd.print("Section 3 Team 1");
 delay(4000);

  dht.begin(); 
              
  Serial.begin(115200);
  
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  // (you can also pass in a Wire library object like &Wire2)
  
}

void loop() {
  unsigned long currentMillis = millis(); // grab current time
  lcd.clear();
 if(isnan(Temperature) || isnan(Humidity) || isnan(moisturePercentage)){
    lcd.clear();
    lcd.setCursor(1, 0); 
    lcd.println("Failed to read Sensor Data check cables!!");
    return;
  }
  
 //humidty  
  Temperature = dht.readTemperature(); // Gets the values of the temperature
  Humidity = dht.readHumidity(); // Gets the values of the humidity 
  lcd.setCursor(0,0);
  lcd.print("TEMP:");
  lcd.setCursor(5,0);
  lcd.print(Temperature);
  
  lcd.setCursor(11,0); 
  lcd.print("HUM:");
  lcd.setCursor(11,1);
  lcd.print(Humidity);
  delay(1000);

//moisture on off
  moisturePercentage = ( 100.00 - ( (analogRead(moisturePin) / 1023.00) * 100.00 ) );

if ((unsigned long)(currentMillis - previousMillis1) >= interval1) {
    Serial.print("Soil Moisture is  = ");
    Serial.print(moisturePercentage);
    Serial.println("%");
    previousMillis1 = millis();
  }

if (moisturePercentage < 50) {
  digitalWrite(motorPin, HIGH);         // tun on motor
  
 
   
   lcd.setCursor(4,1); 
   lcd.print("PUMP:ON");
   delay(1000);
  
}
if (moisturePercentage > 50 && moisturePercentage < 55) {
  digitalWrite(motorPin, HIGH);        //turn on motor pump
   
  lcd.setCursor(4,1);  
  lcd.print("PUMP:ON");
  delay(1000);
}
if (moisturePercentage > 56) {
  digitalWrite(motorPin, LOW);          // turn off mottor
  
  lcd.setCursor(4,1); 
  lcd.print("PUMP:OFF");
  
  delay(1000);
}
  
 
  lcd.setCursor(0,1);     
  lcd.print("M:");
  lcd.setCursor(1, 1); 
  lcd.print(moisturePercentage);
  delay(1000);
  




  //Check WiFi connection status
  if(WiFi.status()== WL_CONNECTED){
    HTTPClient http;
    
    // Your Domain name with URL path or IP address with path
    http.begin(serverName);
    
    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    // Prepare your HTTP POST request data
    String httpRequestData = "api_key=" + apiKeyValue + "&sensor=" + sensorName
                          + "&location=" + sensorLocation + "&value1=" + String(Temperature)
                          + "&value2=" + String(Humidity) + "&value3=" + String(moisturePercentage);
    Serial.print("httpRequestData: ");
    Serial.println(httpRequestData);
    
    
    // Send HTTP POST request
    int httpResponseCode = http.POST(httpRequestData);
     
    
        
    if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();
  }
  else {
    Serial.println("WiFi Disconnected");
  }
  //Send an HTTP POST request every 30 seconds
  delay(30000);  
}
