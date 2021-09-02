

int rele = 6;
int boton = 7;
int estado = LOW;

void setup() {
  pinMode(rele, OUTPUT);
  pinMode(boton, INPUT_PULLUP);
  //digitalWrite(rele,LOW);
 // Serial.begin(9600);
}

void loop() {

  while(digitalRead(boton) == LOW);
 //   digitalWrite(rele, LOW);
    //Serial.println("HOla");
    digitalWrite(rele,LOW);
  while(digitalRead(boton) == HIGH);

  

  digitalWrite(rele, HIGH);
 
}
