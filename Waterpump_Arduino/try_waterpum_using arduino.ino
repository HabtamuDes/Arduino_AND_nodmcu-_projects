#include <ESP8266WiFi.h>
#include <PubSubClient.h>
//______________________________________________________________
// Sensor pins
#define sensorPower 13
#define sensorPin A0
//__________________________________________________________________________________________________
const char* ssid = "what wi-Fi";
const char* password = "12345671";
const char* mqtt_server = "broker.mqttdashboard.com";// MQTT Cloud address
#define water_level_topic "id4"

// Value for storing water level
int val = 0;
///____________________________________________________________________________

WiFiClient espClient;
PubSubClient client(espClient);
//___________________________________________________________________________________
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
//_______________________________________________________________________________________________
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
//_____________________________________________________________________________________________________


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
//________________________________________________________________________________________________________



void setup() {
  // Set D7 as an OUTPUT
  pinMode(sensorPower, OUTPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  // Set to LOW so no power flows through the sensor
  digitalWrite(sensorPower, LOW);
  
  Serial.begin(9600);
}
//___________________________________________________________________________________________
void loop() {
  if (!client.connected()) {
    reconnect();
    }
     client.loop();
  //get the reading from the function below and print it
  int level = readSensor();
  
  Serial.print("Water level: ");
  Serial.println(level);
  
  delay(1000);
  client.publish(water_level_topic, String(level).c_str(), true);
}
//This is a function used to get the reading
int readSensor() {
  digitalWrite(sensorPower, HIGH);  // Turn the sensor ON
  delay(10);              // wait 10 milliseconds
  val = analogRead(sensorPin);    // Read the analog value form sensor
  digitalWrite(sensorPower, LOW);   // Turn the sensor OFF

  return val;             // send current reading
}

