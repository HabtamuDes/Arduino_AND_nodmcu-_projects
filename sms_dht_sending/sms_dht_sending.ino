/*
  commands 
  v = feedback request
 */

#include <SoftwareSerial.h> 
#include "DHT.h"

#define DHTPIN 2     // what pin we're connected to

//Uncomment whatever the type of sensor we are using. 
#define DHTTYPE DHT11   // DHT 11 
//#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is 
// pin3 of the sensor is not connected
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE);

char inchar; // Will hold the incoming character from the GSM shield

SoftwareSerial SIM900(8, 7); // gsm module connected here.

String TextForSms ;
String humidity = " Humidity: %";
String temperature = "   Temperature";
String sign = " *C";


void setup() {

   Serial.begin(9600);
  SIM900.begin(9600); // original 19200
   dht.begin();
pinMode(6, OUTPUT);   
digitalWrite(6, HIGH); 
  
//  delay(10000);  // give time to log on to network.
randomSeed(analogRead(0));

  SIM900.print("AT+CMGF=1\r");  // set SMS mode to text
  delay(1000);
  SIM900.print("AT+CNMI=2,2,0,0,0\r"); 
  // blurt out contents of new SMS upon receipt to the GSM shield's serial out
  delay(1000);
     SIM900.println("AT+CMGD=1,4"); // delete all SMS
   delay(5000);
  Serial.println("Ready...");
  
}




void sendSMS(String message)
{
  SIM900.println("AT+CMGF=1\r");                     // AT command to send SMS message
  delay(1000);
 SIM900.println("AT+CMGS = \"+251938650537\"");  // recipient's mobile number, in international format
 
  delay(1000);
  SIM900.println(message);                         // message to send
  delay(1000);
  SIM900.println((char)26);                        // End AT command with a ^Z, ASCII code 26
  delay(1000); 
  SIM900.println();
  delay(1000);                                     // give module time to send SMS
                                   // turn off module
}
void loop() {

if(SIM900.available() == 0)
{

   // Wait a few seconds between measurements.
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  int h = dht.readHumidity();
  // Read temperature as Celsius
  int t = dht.readTemperature();
  // Read temperature as Fahrenheit
  int f = dht.readTemperature(true);
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index
  // Must send in temp in Fahrenheit!
  int hi = dht.computeHeatIndex(f, h);

//  Serial.print("Humidity: "); 
//  Serial.print(h);
//  Serial.print(" %\t");
//  Serial.print("Temperature: "); 
//  Serial.print(t);
//  Serial.print(" *C ");
  
  TextForSms = TextForSms + "Humidity: ";
  TextForSms.concat(h);
  TextForSms = TextForSms + "%    Temperature: ";
  TextForSms.concat(t);
  TextForSms = TextForSms + "*C";
  Serial.println(TextForSms);
  delay(2000);
  TextForSms = " ";
  
   if ( t > 40 )
   {
   Serial.println("Temperature Exceeded"); 
TextForSms = " Temperature Exceeded";
 sendSMS(TextForSms);
 delay(5000); 
 TextForSms = ""; 
   }
  
}
  if(SIM900.available() >0)
  {

    inchar=SIM900.read(); 
  Serial.println(inchar);
    delay(20);
    if (inchar=='h')
    {
      delay(10);
 
   Serial.println(inchar);
    
  // Wait a few seconds between measurements.
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  int h = dht.readHumidity();
  // Read temperature as Celsius
  int t = dht.readTemperature();
  // Read temperature as Fahrenheit
  int f = dht.readTemperature(true);
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index
  // Must send in temp in Fahrenheit!
  int hi = dht.computeHeatIndex(f, h);

//  Serial.print("Humidity: "); 
//  Serial.print(h);
//  Serial.print(" %\t");
//  Serial.print("Temperature: "); 
//  Serial.print(t);
//  Serial.print(" *C ");
  
  TextForSms = TextForSms + "HUMIDITY: ";
  TextForSms.concat(h);
  TextForSms = TextForSms + "%    TEMPERATURE: ";
  TextForSms.concat(t);
  TextForSms = TextForSms + "*C";
   sendSMS(TextForSms);
  Serial.println(TextForSms);
  delay(2000);
  TextForSms = " ";
    }
 
}


}
