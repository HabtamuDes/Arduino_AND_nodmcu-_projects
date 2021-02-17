#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define DHTTYPE DHT11 
// DHT Sensor
uint8_t DHTPin = D4; 


// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);                

float t;
float h;

///////////////////// SENDING DATA ON DATABASE/////////////////////////////////


const int soil = A0;

const char* ssid     = "GERD";
const char* password = "";

const char* host = "192.168.43.230";
String url;

unsigned long delayStart = 0; // the time the delay started
unsigned long timeInterval = 0;

void setup() {

  pinMode(DHTPin, INPUT);

  dht.begin(); 
              
  lcd.begin(16,2);
  lcd.init();
  lcd.backlight();
 
  Serial.begin(115200);
  delay(100);
  /////////////////////// CONTROL PIN///////////////////
  pinMode(soil, INPUT);
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

  url ="/try/motor/read.php?id=568";
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

  //humidty  
  t= dht.readTemperature(); // Gets the values of the temperature
  h= dht.readHumidity(); // Gets the values of the humidity 
  lcd.setCursor(1, 0);      
  lcd.print("temperature= ");
  lcd.print(t);
  
  lcd.setCursor(2, 1); 
  lcd.print("Humidity= ");
  lcd.print(h);
  delay(1000);
  // Read moisture sensor value
  int mos = analogRead(soil);
  
  lcd.setCursor(2, 1); 
  lcd.print("moisture= ");
  lcd.print(mos);

  Serial.print("connecting to ");
  Serial.println(host);

  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  //String url = "/farmapi/insert.php?temp=" + String(t) + "&hum="+ String(h);
  //String url = "/api/moisture/insert.php?mos=" + String(mos);
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
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
