int RelayPin = D2;


void setup(){
  //set relay as an output pin
  pinMode(RelayPin, OUTPUT);
  
  }

  void loop(){
    
    //lets turn on the relay
    digitalWrite(RelayPin, LOW);
    delay(3000);

    //lets turn of the relay
    digitalWrite(RelayPin, HIGH);
    delay(3000);
    }
