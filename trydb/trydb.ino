///////////////Written by Eyob Tilahun/////////////////


#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define DHTTYPE DHT11 
// DHT Sensor
uint8_t DHTPin = D4; 
int farmid=2;
int sensorid=4;

// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);                

float t;
float h;

///////////////////// SENDING DATA ON DATABASE/////////////////////////////////


const int soil = A0;

const char* ssid     = "try1";
const char* password = "12345678";

const char* host = "192.168.43.129";
String url;


/////////////////////////////////Device Configuration for Maintainers//////////////////////////////////////////
                    //////Default Value for farmid and sensoris is 0 ///////////////////////////////////////

                            

//////////////////////////////////////////////////////////////////////////////////////////////////////////////



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
    Serial.print(".cc");
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
 // delayStart = millis();   // start delay
  //timeInterval = 2000;


}


void loop()
{
  
float temperature = dht.readTemperature();
float humidity = dht.readHumidity();  
float moisture = analogRead(soil);
  

  if(isnan(temperature) || isnan(humidity) || isnan(moisture)){
    Serial.println("Failed to read DHT11");
  }else{
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.print(" %\t");

    Serial.print("Moisture: ");
    Serial.print(moisture);
    Serial.print(" %\t");

    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" *C");
    delay(3000);
  }
   
    Serial.print("connecting to ");
    Serial.println(host);

    // Use WiFiClient class to create TCP connections
    WiFiClient client;
    const int httpPort = 5555;
    if (!client.connect(host, httpPort)) {
        Serial.println("connection failed");
        return;
    }

 


    // This will send the request to the server
 client.print(String("GET http://192.168.43.129/insert.php?") + 
                          ("&temp=") + temperature +
                           ("&moisture=") + moisture +
                          ("&humidity=") + humidity +
                          ("&sensorid=") + sensorid +
                          ("&farmid=") + farmid +
                          " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while (client.available() == 0) {
        if (millis() - timeout > 1000) {
            Serial.println(">>> Client Timeout !");
            client.stop();
            return;
        }
    }

    // Read all the lines of the reply from server and print them to Serial
    while(client.available()) {
        String line = client.readStringUntil('\r');
        Serial.print(line);
        
    }

    Serial.println();
    Serial.println("closing connection");
}
