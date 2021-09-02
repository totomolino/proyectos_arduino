void setup() {
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  if(digitalRead(6) == LOW){
    Serial.println("TE");
  }
  if(digitalRead(7) == LOW){
    Serial.println("AMO");
  }
  delay(100);
}
