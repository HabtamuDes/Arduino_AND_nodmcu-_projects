#include <DHT.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define DHTPIN 7
#define DHTTYPE DHT11 //21 or 22 also an option
DHT dht(DHTPIN, DHTTYPE);

unsigned long readTime;

// Update these with values suitable for your network.

const char* ssid = "what wi-Fi";
const char* password = "12345671";
const char* mqtt_server = "broker.mqtt-dashboard.com";
char message_buff[100];
WiFiClient espClient;
PubSubClient client(espClient);



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

void callback(char* topic, byte* payload, unsigned int length) {
Serial.print("Message arrived [");
Serial.print(topic);
Serial.print("] ");
int i=0;
for (i=0;i<length;i++) {
  Serial.print((char)payload[i]);
  message_buff[i] = payload[i];
}
message_buff[i] = '\0';
String msgString = String(message_buff);
if (msgString.equals("OFF")) {
  client.publish("openhab/himitsu/command","acknowedging OFF");
}
else if(msgString.equals("ON")){
  client.publish("openhab/himitsu/command","acknowedging ON");
}
Serial.println();
}

void reconnect() {
// Loop until we're reconnected
while (!client.connected()) {
  Serial.print("Attempting MQTT connection...");
  // Create a random client ID
  String clientId = "ESP8266Client-";
  clientId += String(random(0xffff), HEX);
  // Attempt to connect
  if (client.connect(clientId.c_str())) {
    Serial.println("connected");
    
    // Once connected, publish an announcement...
    client.publish("openhab","himitsu sensor, reporting in");
    // ... and resubscribe
    client.subscribe("openhab/himitsu/command");
  } else {
    Serial.print("failed, rc=");
    Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void loop()
{
  
  if (!client.connected()) {
    reconnect();
  }
  
  client.loop();
  
  //check if 5 seconds has elapsed since the last time we read the sensors. 
  if(millis() > readTime+60000){
    sensorRead();
  }
  
}

void sensorRead(){
  readTime = millis();
 // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  char buffer[10];
  dtostrf(t,0, 0, buffer);
  client.publish("openhab/himitsu/temperature",buffer);
  //Serial.println(buffer);
  dtostrf(h,0, 0, buffer);
  client.publish("openhab/himitsu/humidity",buffer);
  
  //client.publish("inTopic/humidity",sprintf(buf, "%f", h));
  /*Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.print(" *F\t");
  Serial.print("Heat index: ");
  Serial.print(hic);
  Serial.print(" *C ");
  Serial.print(hif);
  Serial.println(" *F"); */
}


