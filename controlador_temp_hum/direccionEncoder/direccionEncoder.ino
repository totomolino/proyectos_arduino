#include <LiquidCrystal_I2C.h>
#include <ClickEncoder.h>
#include <TimerOne.h>
int16_t oldEncPos, encPos;
uint8_t buttonState;

#define pinA A2 //CLK
#define pinB A1 //DW
#define pinSw A0 //switch
#define STEPS 4
#define dhtPin 6

LiquidCrystal_I2C lcd(0x27,16,2);

ClickEncoder encoder(pinA, pinB, pinSw, STEPS);

void setup() {

    //LCD
    lcd.init();
    lcd.backlight();
    //NOse
    Timer1.initialize(1000);
    Timer1.attachInterrupt(timerIsr);
    
    encoder.setAccelerationEnabled(true);
  
    //delay(1000);
    lcd.clear();
    oldEncPos = -1;
}

void loop(){

  
/*
  if (dirEncoder() == -1){
    lcd.print("izq");
    lcd.clear();
    delay(100);
  }
  if (dirEncoder() == 1){
    lcd.print("der");
    lcd.clear();
    delay(100);
  }*/
  
  encPos += encoder.getValue();

  if (encPos != oldEncPos) {
    if((oldEncPos - encPos)<0){
      lcd.clear(); 
      lcd.print("Derecha");
      
    }
    if((oldEncPos - encPos)>0){
      lcd.clear();
      lcd.print("Izquierda");
       
    }
    oldEncPos = encPos;
  }
  /*
  int res = dirEncoder();

  if(res){
    lcd.clear();
    lcd.print("derecha");
  }else{
    lcd.clear();
    lcd.print("izquierda");
  }*/
}

int dirEncoder(){//devuelve 1 si gira para la derecha, y -1 si para la izq
  encPos += encoder.getValue();
  int res;
  if (encPos != oldEncPos) {
    if((oldEncPos - encPos)<0){
      //lcd.clear(); 
      //lcd.print("Derecha");
      res = 1;
    }
    if((oldEncPos - encPos)>0){
     // lcd.clear();
      //lcd.print("Izquierda");
      res = 0; 
    }
    oldEncPos = encPos;
  }

  return res;
}
void botonEncoder(){
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
          
        break;

      case ClickEncoder::DoubleClicked: //6
        break;
    }
  }
}










void timerIsr() {
  encoder.service();
}
