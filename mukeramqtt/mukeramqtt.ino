More information about projects PDAControl 
* PDAControl English http://pdacontrolen.com 
* Mas informacion sobre proyectos PDAControl
* PDAControl Espanol http://pdacontroles.com 
* Channel Youtube https://www.youtube.com/c/JhonValenciaPDAcontrol/videos 
*/
 
#include < ESP8266WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include  <DallasTemperature.h>
 
// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2 // pin 2
 
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
 
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&amp;oneWire);
 
// Update these with values suitable for your network.
 
const char* ssid = "raspi-gui";
const char* password = "ChangeMe";
const char* mqtt_server = "broker.hivemq.com"; /// MQTT Broker
int mqtt_port = 1883;
 
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
 
void setup() { 
 Serial.begin(115200);
 // Start up the library
 sensors.begin();
 
 setup_wifi();
 client.setServer(mqtt_server, mqtt_port);
 client.setCallback(callback);
 
 Serial.println("Connected ");
 Serial.print("MQTT Server ");
 Serial.print(mqtt_server);
 Serial.print(":");
 Serial.println(String(mqtt_port)); 
 Serial.print("ESP8266 IP ");
 Serial.println(WiFi.localIP()); 
 Serial.println("Modbus RTU Master Online");
 
 
}
 
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
 
 Serial.println("");
 Serial.println("WiFi connected");
 Serial.println("IP address: ");
 Serial.println(WiFi.localIP());
}
 
void callback(char* topic, byte* payload, unsigned int length) {
 Serial.print("Message arrived [");
 Serial.print(topic);
 Serial.print("] ");
 for (int i = 0; i &lt; length; i++) {
 Serial.print((char)payload[i]);
 }
 Serial.println();
 
}
 
void reconnect() {
 // Loop until we're reconnected
 while (!client.connected()) {
 Serial.print("Attempting MQTT connection...");
 // Attempt to connect
 if (client.connect("ESP8266Client")) {
 
 Serial.println("connected");
 // client.subscribe("event");
 } else {
 Serial.print("failed, rc=");
 Serial.print(client.state());
 Serial.println(" try again in 5 seconds");
 // Wait 5 seconds before retrying
 delay(5000);
 }
 }
}
void loop() {
 
 sensors.requestTemperatures();
 float celsius = sensors.getTempCByIndex(0);
 Serial.println(sensors.getTempCByIndex(0));
 
 char temperaturenow [15];
 dtostrf(celsius,7, 3, temperaturenow); //// convert float to char 
 client.publish("temperature/PDAControl/sensor", temperaturenow); /// send char 
 
 if (!client.connected()) {
 reconnect();
 
 }
 client.loop();
 
 delay(10000);
 
}
