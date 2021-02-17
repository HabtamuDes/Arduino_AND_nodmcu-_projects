#include <ESP8266WiFi.h>
​
#include <PubSubClient.h>
​
#include "DHT.h"
​
#define DHTPIN D1     // what pin we're connected to
#define wifi_ssid "what wi_Fi"
#define wifi_password "12345671"
​
#define mqtt_server "broker.mqttdashboard.com"  // MQTT Cloud address
#define humidity_topic "humidity"
#define temperature_topic "temperature"
​
#define DHTTYPE DHT11   // DHT 11
WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);
void setup() {
    Serial.begin(115200);
    setup_wifi();
    client.setServer(mqtt_server, 1883);
    dht.begin();
}
void setup_wifi() {
    delay(10);
    WiFi.begin(wifi_ssid, wifi_password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
}
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
bool checkBound(float newValue, float prevValue, float maxDiff) {
    return newValue < prevValue - maxDiff || newValue > prevValue + maxDiff;
}
long lastMsg = 0;
float temp = 0.0;
float hum = 0.0;
float diff = 1.0;
void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();
long now = millis();
    if (now - lastMsg > 30000) {
        // Wait a few seconds between measurements
        lastMsg = now;
float newTemp = dht.readTemperature();
float newHum = dht.readHumidity();
        if (checkBound(newTemp, temp, diff)) {
            temp = newTemp;
            Serial.print("New temperature:");
            Serial.println(String(temp).c_str());
            client.publish(temperature_topic, String(temp).c_str(), true);
        }
if(checkBound(newHum, hum, diff)){
            hum = newHum;
            Serial.print("New humidity:");
            Serial.println(String(hum).c_str());
            client.publish(humidity_topic, String(hum).c_str(), true);
        }
    }
}
