#include <PubSubClient.h>

#include <PubSubClient.h>
#include <MQTT.h>
#include <PubSubClient_JSON.h>





#include <ESP8266WiFi.h>
 
#include <stdlib.h>
const char *ssid =  "what wi-Fi";   // cannot be longer than 32 characters!
const char *pass =  "12345671";   //
const char *mqtt_server = "broker.mqttdashboard.com";
const int mqtt_port = 8000;
const char *mqtt_user = "abebe";
const char *mqtt_pass = "1234";
const char *mqtt_client_name = "e"; // Client connections cant have the same connection name
#define BUFFER_SIZE 100
unsigned long previousMillis = 0;
const long interval = 10000;   
WiFiClient wclient;  //Declares a WifiClient Object using ESP8266WiFi
PubSubClient client(wclient, mqtt_server,  mqtt_port);  //instanciates client object
//Function is called when, a message is recieved in the MQTT server.
void callback(const MQTT::Publish& pub) {
Serial.print(pub.topic());
Serial.print(" => ");
 if (pub.has_stream()) {
    uint8_t buf[BUFFER_SIZE];
    int read;
    while (read = pub.payload_stream()->read(buf, BUFFER_SIZE)) {
      Serial.write(buf, read);
    }
    pub.payload_stream()->stop();
 
    //Check if the buffer is -1
    if(buf[0]=='-' && buf[1]=='1'){
    //Code to blink the LED -- its strange that I can't blink the LED for more than 1sec.
       digitalWrite(LED_BUILTIN, HIGH);
       delay(1000);
       digitalWrite(LED_BUILTIN, LOW);  
    }
     
    Serial.println("");
  }
Serial.println(pub.payload_string());
if(pub.payload_string()=="on"){
//Code to blink the LED -- its strange that I can't blink the LED for more than 1sec.
 digitalWrite(LED_BUILTIN, LOW);
 delay(4000);
Serial.println("led on");
}
if(pub.payload_string()=="off"){
//Code to blink the LED -- its strange that I can't blink the LED for more than 1sec.
 digitalWrite(LED_BUILTIN, HIGH);
 delay(4000);
Serial.println("led off");
}
}

void setup() {
// Setup console
Serial.begin(115200);  //set the baud rate
delay(10);
pinMode(LED_BUILTIN, OUTPUT); 
 digitalWrite(LED_BUILTIN, LOW);
  delay(2000);                      // Wait for a second
  digitalWrite(LED_BUILTIN, HIGH);
Serial.println();
Serial.println();
}
void loop() {
if (WiFi.status() != WL_CONNECTED) {  //wifi not connected?
Serial.print("Connecting to ");
Serial.print(ssid);
Serial.println("...");
WiFi.begin(ssid, pass);
if (WiFi.waitForConnectResult() != WL_CONNECTED)
return;
Serial.println("WiFi connected");
}
if (WiFi.status() == WL_CONNECTED) {
//client object makes connection to server
if (!client.connected()) {
Serial.println("Connecting to MQTT server");
//Authenticating the client object
if (client.connect(MQTT::Connect("mqtt_client_name")
.set_auth(mqtt_user, mqtt_pass))) {
Serial.println("Connected to MQTT server");
//Subscribe code
client.set_callback(callback);
client.subscribe("Temparture-sensor");
} else {
Serial.println("Could not connect to MQTT server");   
}
}
if (client.connected())
client.loop();
}
//SendTempHumid();  // this will send the dummy temparature reading
}
// Non-Blocking delay
void SendTempHumid(){
unsigned long currentMillis = millis();
if(currentMillis - previousMillis >= interval) {  // checks if 10 delay is over
// save the last time you read the sensor 
previousMillis = currentMillis;   
srand(currentMillis); //create a random value based on time
int h = rand()%100; // sets value between 0-99
if (isnan(h)) {
Serial.println("Failed to read from DHT sensor!");
//return; This will ensure that data is always sent
}
Serial.print("Temparture-sensor"); 
Serial.print(h);
//client.publish("Temparture-sensor",String(h) );
}
}
