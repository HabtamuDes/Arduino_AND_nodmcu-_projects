int RelayPin = D0;

void setup() {
  Serial.begin(115200);
  // Set RelayPin as an output pin
  pinMode(RelayPin, OUTPUT);
}

void loop() {
  // Let's turn on the relay...
  digitalWrite(RelayPin, LOW);
  delay(3000);
  Serial.println("its in the loop");
  // Let's turn off the relay...
  digitalWrite(RelayPin, HIGH);
  delay(6000);
}
