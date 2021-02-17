#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"
//_____________________________________________________________________________________________________

const int sensor_pin = A0;  /* Connect Soil moisture analog sensor pin to A0 of NodeMCU */
#define DHTPIN 0     // D3 what pin we're connected to
const char* ssid = "what wi-Fi";
const char* password = "12345671";
const char* mqtt_server = "broker.mqttdashboard.com";// MQTT Cloud address
#define humidity_topic "id8"
#define temperature_topic "id6"
#define soil_moisture_topic "id7"
#define DHTTYPE DHT11   // DHT 11
//_________________________________________________________________________________________________________
WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);
long lastMsg = 0;
float temp = 0.0;
float hum = 0.0;
float diff = 1.0;
//_________________________________________________________________________________________________________________

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
//_________________________________________________________________________________________________________________
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}
//___________________________________________________________________________________________________________________
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("nodeMcuDHT11")) {
      Serial.println("connected");
      } else {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        delay(5000);
        }
    }
}

//_____________________________________________________________________________________________________________________
bool checkBound(float newValue, float prevValue, float maxDiff) {
  return newValue < prevValue - maxDiff || newValue > prevValue + maxDiff;
}
//______________________________________________________________________________________________________________________

void setup() {
  Serial.begin(115200);
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  dht.begin();
}

//_______________________________________________________________________________________________________________________

void loop() {
  if (!client.connected()) {
    reconnect();
    }
    client.loop();
   long now = millis();
   if (now - lastMsg > 3000) {
      // Wait a few seconds between measurements
   lastMsg = now;
      
    float newTemp = dht.readTemperature();
    float newHum = dht.readHumidity();
   if(checkBound(newTemp, temp, diff)) {
        temp = newTemp;
        Serial.print("New temperature:");
        Serial.println(String(temp).c_str());
        client.publish(temperature_topic, String(temp).c_str(), true);
        }
    if (checkBound(newHum, hum, diff)) {
          hum = newHum;
          Serial.print("New humidity:");
          Serial.println(String(hum).c_str());
          client.publish(humidity_topic, String(hum).c_str(), true);
        }
    }
  
  
  float moisture_percentage;
  moisture_percentage = ( 100.00 - ( (analogRead(sensor_pin)/1023.00) * 100.00 ) );
  Serial.print("Soil Moisture(in Percentage) = ");
  Serial.print(moisture_percentage);
  Serial.println("%");
  client.publish(soil_moisture_topic, String(moisture_percentage).c_str(), true);
  delay(1000);
}


