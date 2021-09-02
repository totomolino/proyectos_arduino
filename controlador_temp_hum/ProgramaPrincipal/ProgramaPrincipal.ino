#include <LiquidCrystal_I2C.h>
#include <ClickEncoder.h>
#include <TimerOne.h>
#include <DHT.h>
#include <DHT_U.h>


int16_t oldEncPos, encPos;
uint8_t buttonState;

#define pinA A2
#define pinB A1
#define pinSw A0 //switch
#define STEPS 4
#define dhtPin 6
int rele = 7;
float TEMP;
float HUM;


LiquidCrystal_I2C lcd(0x27,16,2);

ClickEncoder encoder(pinA, pinB, pinSw, STEPS);

DHT dht(dhtPin,DHT22);

void setup() {
    lcd.init();
    lcd.backlight();
    Timer1.initialize(1000);
    Timer1.attachInterrupt(timerIsr);
    pinMode(rele,OUTPUT);
    //pinMode(dhtPin,INPUT_PULLUP);
    encoder.setAccelerationEnabled(true);
    dht.begin();

    lcd.setCursor(0,0);
    lcd.print("Acceleration is ");
    lcd.setCursor(0,1);
    //lcd.print((encoder.getAccelerationEnabled()) ? "enabled" : "disabled");
    //delay(1000);
    lcd.clear();
    oldEncPos = -1;
}

void loop() {
  
  
  /*  encPos += encoder.getValue();

  if (encPos != oldEncPos) {
    oldEncPos = encPos;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Encoder Value: ");
    lcd.setCursor(0,1);
    lcd.print(encPos);
  }*/

    delay(2000);
    TEMP = dht.readTemperature();
    HUM = dht.readHumidity();
    lcd.setCursor(0,0);
    lcd.print("Temp: ");
    lcd.print(TEMP);
    lcd.setCursor(0,1);
    lcd.print("Hum: %");
    lcd.print(HUM);

    if(TEMP < 23.6){
      digitalWrite(rele, HIGH);
    }else
    digitalWrite(rele, LOW);
  
  buttonState = encoder.getButton();

  

  if (buttonState != 0) {
    lcd.clear();
    lcd.setCursor(0,0);
    //lcd.print("Button: "); lcd.print(buttonState);
    switch (buttonState) {
      case ClickEncoder::Open:          //0
        break;

      case ClickEncoder::Closed:        //1
        break;

      case ClickEncoder::Pressed:       //2
        break;

      case ClickEncoder::Held:          //3
        break;

      case ClickEncoder::Released:      //4
        break;

      case ClickEncoder::Clicked:       //5
        digitalWrite(rele,HIGH);
        delay(500);
        digitalWrite(rele,LOW);
        break;

      case ClickEncoder::DoubleClicked: //6
        break;
    }
  }

}

void timerIsr() {
  encoder.service();
}
