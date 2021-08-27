#include <DHT.h>
#include <DHT_U.h>
#include <LiquidCrystal.h>

int sensor = 2; //se pone el pin donde esta el sensor

int temperatura;
int humedad;
DHT dht(sensor, DHT22);

//Reles

int rele1 = 1; //se pone el pin donde esta el rele

//Pantalla

LiquidCrystal lcd(7,6,5,4,3,2);


void setup() {


}

void loop() {
  //dht.readTemperature(); //con esta funcion leo la temperatura
  //dht.readHumidity(); //con esta funcion leo la humedad
  
  
    
}


void encenderRele(int unRele){
  digitalWrite(unRele, LOW);
}


void apagarRele(int unRele){
  digitalWrite(unRele, HIGH);
}

void imprimirEnLcd(String texto){
  lcd.print(texto);
}
