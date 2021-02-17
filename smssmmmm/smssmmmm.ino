
#include <SoftwareSerial.h>
SoftwareSerial mySerial(7,8);
char msg;
char call;
int buttonpin=12;
void setup()
{
  pinMode(buttonpin,INPUT);
  mySerial.begin(9600);   // Setting the baud rate of GSM Module  
  Serial.begin(9600);// Setting the baud rate of Serial Monitor (Arduino)
  Serial.println("press button");
}

void loop()
{
  if(digitalRead(buttonpin)==HIGH)
  {
    
    Serial.println("button pressed");
    delay(2000);
    SendMessage();  
  }

 if (mySerial.available()>0)
 Serial.write(mySerial.read());
}

void SendMessage()
{
  mySerial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second

  mySerial.println("AT+CMGS=\"+xxxxxxxxxxxxx\"\r"); // Replace x with mobile number
  delay(1000);

  mySerial.println("LOW CGPA DUDE....");// The SMS text you want to send
  delay(100);
   mySerial.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
}
