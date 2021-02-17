#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#include <ESP8266HTTPClient.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);


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


///////////////////// SENDING DATA ON DATABASE/////////////////////////////////



// Replace with your network credentials
const char* ssid     = "GERD";
const char* password = "";

// REPLACE with your Domain name and URL path or IP address with path
const char* host = "192.168.43.230";

String url;

// Keep this API Key value to be compatible with the PHP code provided in the project page. 
// If you change the apiKeyValue value, the PHP file /post-esp-data.php also needs to have the same key 
String apiKeyValue = "tPmAT5Ab3j7F9";

int sensorName = 1;
int sensorLocation = 16;
 

unsigned long delayStart = 0; // the time the delay started
unsigned long timeInterval = 0;

void setup() {

  pinMode(DHTPin, INPUT);
  
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
 delay(2000);
 
 lcd.clear(); 
 lcd.setCursor(0,0);
 lcd.print("TEAM MEMBERS ");
 lcd.setCursor(0,1);
 lcd.print("HABTAMU 059/16");
 delay(2000);

 lcd.clear(); 
 lcd.setCursor(0,0);
 lcd.print("EYOB  044/16 ");
 lcd.setCursor(0,1);
 lcd.print("GUTEMA 058/16");
 delay(2000);
 
 
 lcd.clear(); 
 lcd.setCursor(0,0);
 lcd.print("MULU  085/16 ");
 lcd.setCursor(0,1);
 lcd.print("TESFAZER 049/16");
 delay(2000);
 

  
 
  Serial.begin(115200);
  delay(100);
  /////////////////////// CONTROL PIN///////////////////
  pinMode( moisturePin, INPUT);
  pinMode(D0, OUTPUT); ////motor pin D0 pin in Nodemcu
  
  digitalWrite(D0, HIGH); //High is Low, Low is High
  Serial.println();
  Serial.println();
  //////////////////////////////////////////

  Serial.println();

  Serial.print("Connecting to ");
  Serial.println(ssid);



  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Netmask: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("Gateway: ");
  Serial.println(WiFi.gatewayIP());

  ///////////////////////////// CONTROL PIN ///////////////////////////////
  digitalWrite(D0, LOW);
  delay(500);
  digitalWrite(D0, HIGH);
  delay(500);
  ////////////////////////////////////////////////////////////
  delayStart = millis();   // start delay
  timeInterval = 2000;


}

void controlPin() {
  int motor_on;
  int delay_value;
  int user_min; //user min
  int user_max; //user max
  int cur_value; //current value
  int def_min = 340; //default value
  int def_max = 921; //default value
  String defMode = "MAN";
  Serial.print("connecting to.. ");
  Serial.println(host);

  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }


  url = "/try/motor/read.php?id=568";
  Serial.println("Sending GET Request..");

  Serial.print("Requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.0\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  delay(500);
  String section = "header";
  while (client.available()) {
    String line = client.readStringUntil('\r');
    //Serial.print(line);
    // we’ll parse the HTML body here
    if (section == "header") { // headers..

      if (line == "\n") { // skips the empty space at the beginning
        section = "json";
      }
    }
    else if (section == "json") { // print the good stuff
      section = "ignore";
      String result = line.substring(1);
      Serial.println("result = " + result);

      // Parse JSON
      int size = result.length() + 1;
      char json[size];
      result.toCharArray(json, size);
      StaticJsonBuffer<220> jsonBuffer;
      JsonObject& json_parsed = jsonBuffer.parseObject(json);

      if (!json_parsed.success())
      {
        Serial.println(json);
        Serial.println("parseObject() failed");
        return;
      }
      String motor1 = json_parsed["motor"][0]["Status"];
      String delay_val = json_parsed["motor"][0]["delay"];
      String new_val = json_parsed["motor"][0]["new_value"];
      String usr_min = json_parsed["motor"][0]["min"];
      String usr_max = json_parsed["motor"][0]["max"];
      String modeOn = json_parsed["motor"][0]["mode"];
      delay_value = (delay_val).toInt();
      cur_value = (new_val).toInt();
      motor_on = (motor1).toInt();
      user_min = (usr_min).toInt();
      user_max = (usr_max).toInt();

      if(user_min==0){
        user_min = def_min;
      }      
      if(user_max==0){
        user_max = def_max;
      }
      if(modeOn == ""){
        modeOn = defMode;
      }
      
      //String motor2 = json_parsed["motor"][1]["status"];
      Serial.println("motor = " + motor_on);
      Serial.print("current value = "); Serial.println(cur_value);
      Serial.print("minimum = "); Serial.println( user_min);
      Serial.print("maximum = "); Serial.println( user_max);
      Serial.println("mode = " + modeOn);
      


      ///////////////// CONTROL PIN /////////////////////
      if(modeOn == "AUTO"){
        if(cur_value>user_min && cur_value<user_max){
          Serial.print("Motor is On (Auto Mode)");
          digitalWrite(D0, HIGH);
        }else if(cur_value<user_min || cur_value>user_max){
          Serial.print("Motor is Off (Auto Mode)");
          digitalWrite(D0, LOW);
        }
      }else if(modeOn == "MAN"){
          if (motor_on == 0) {
          digitalWrite(D0, LOW);
          Serial.println("Motor is Off (Manual Mode)");
        }
        else if (motor_on == 1) {
          digitalWrite(D0, HIGH);
          Serial.println("Motor is ON(Manual Mode)");
        }
      }
    }
  }
  Serial.println();
  Serial.println("closing connection");
  delay(delay_value);
}

void loop() {

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

  // Read moisture sensor value
   moisturePercentage = ( 100.00 - ( (analogRead(moisturePin) / 1023.00) * 100.00 ) );
 
  
 
  lcd.setCursor(0,1);     
  lcd.print("MOI:");
  lcd.setCursor(4, 1); 
  lcd.print(moisturePercentage);
 

  Serial.print("connecting to ");
  Serial.println(host);

  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  if(WiFi.status()== WL_CONNECTED){
    HTTPClient http;
    
    // Your Domain name with URL path or IP address with path
    http.begin("http://192.168.43.230/try/post-esp-data.php");
    
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
  
    client.print(String("GET ") + url + " HTTP/1.0\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");


  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  Serial.println();
  Serial.println("closing connection");
  //  if((millis() - delayStart) >= timeInterval){
  //    delayStart = millis();
  //  }
  delay(10000);//2min=120000, 1.5min=90000
  controlPin();

}
