#include <LiquidCrystal_I2C.h>
#include <ClickEncoder.h>
#include <TimerOne.h>
#include <DHT.h>
#include <DHT_U.h>


int16_t oldEncPos, encPos;
uint8_t buttonState;

#define pinA A2 //CLK
#define pinB A1 //DW
#define pinSw A0 //switch
#define STEPS 4
#define dhtPin 7
#define boton 2
int rele = 7;
float TEMP;
float HUM;
int menu = 1;
int maxT = 25;//Max temperatura
int minT = 10; // Min temperatura
int dT = 2; // histeresis temperatura(delta T)
int maxH = 60; //Max humedad
int minH = 20; //Min humedad
int dH = 2; // histeresis humedad(delta H)
int menuConfig = 0;
void (*funcion)(void);

LiquidCrystal_I2C lcd(0x27,16,2);

ClickEncoder encoder(pinA, pinB, pinSw, STEPS);

DHT dht(dhtPin,DHT22);

void setup() {
    lcd.init();
    lcd.backlight();
    Timer1.initialize(1000);
    Timer1.attachInterrupt(timerIsr);
    pinMode(rele,OUTPUT);
    pinMode(boton, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(boton),cambiarFuncion,RISING);
    encoder.setAccelerationEnabled(true);
    dht.begin();

    //delay(1000);
    lcd.clear();
    oldEncPos = 0;
    inicializar();
    lcd.clear();
    //updateMenu();
    funcion = showDht;

}

void loop() {

  funcion();
      
}

void cambiarFuncion(){
  funcion = configuracion;
  //detachInterrupt(digitalPinToInterrupt(boton));
  
}



void timerIsr() {
  encoder.service();
}

void menuPpal(){
  //updateMenuPpal();
  if(!digitalRead(boton)){
    menuConfig = 1;
    menu = 1;
    updateMenu();
    while(menuConfig == 1)
    {
      
      configuracion();
    }
    while (!digitalRead(boton));
  }
}

void configuracion(){
  detachInterrupt(digitalPinToInterrupt(boton));
  boolean quieroSeguir = true;
  menu = 1;
  updateMenu();
  
  while(quieroSeguir){
    encPos += encoder.getValue();
  
    if (encPos != oldEncPos) {
      if((oldEncPos - encPos)>0){
           menu--;
           updateMenu();
           delay(100);  //der    
      }
      if((oldEncPos - encPos)<0){
          menu++;
          updateMenu();
          delay(100);//izq
      }
      oldEncPos = encPos;
  
    }
     
    if (botonEncoder() == 1){//Seleccionar
      executeAction();
      updateMenu();
      delay(100);
      while (botonEncoder() == 1);
    }
    if (!digitalRead(boton)){//Salir
      quieroSeguir = false;
      while(!digitalRead(boton));
      delay(1000);
      attachInterrupt(digitalPinToInterrupt(boton),cambiarFuncion,RISING);
    }
  }

}

void showDht(){
    
    TEMP = dht.readTemperature();
    HUM = dht.readHumidity();
    lcd.setCursor(0,0);
    lcd.print("Temp: ");
    lcd.print(TEMP);
    lcd.setCursor(0,1);
    lcd.print("Hum: %");
    lcd.print(HUM);
    //delay(500);
    
}

void updateMenu() {
  switch (menu) {
    case 0:
      menu = 1;
      break;
    case 1:
      lcd.clear();
      lcd.print(">Max Temp:");
      lcd.setCursor(12,0);
      imprimirVariable(maxT);
      lcd.setCursor(0, 1);
      lcd.print(" Min Temp:");
      lcd.setCursor(12,1);
      imprimirVariable(minT);
      break;
    case 2:
      lcd.clear();
      lcd.print(" Max Temp:");
      lcd.setCursor(12,0);
      imprimirVariable(maxT);
      lcd.setCursor(0, 1);
      lcd.print(">Min Temp:");
      lcd.setCursor(12,1);
      imprimirVariable(minT);
      break;
    case 3:
      lcd.clear();
      lcd.print(">D Temp:");
      lcd.setCursor(12,0);
      imprimirVariable(dT);
      lcd.setCursor(0, 1);
      lcd.print(" Max hum:");
      lcd.setCursor(12,1);
      imprimirVariable(maxH);
      break;
    case 4:
      lcd.clear();
      lcd.print(" D Temp:");
      lcd.setCursor(12,0);
      imprimirVariable(dT);
      lcd.setCursor(0, 1);
      lcd.print(">Max hum:");
      lcd.setCursor(12,1);
      imprimirVariable(maxH);
      break;
     case 5:
      lcd.clear();
      lcd.print(">Min hum:");
      lcd.setCursor(12,0);
      imprimirVariable(minH);
      lcd.setCursor(0, 1);
      lcd.print(" D hum:");
      lcd.setCursor(12,1);
      imprimirVariable(dH);
      break;
    case 6:
      lcd.clear();
      lcd.print(" Min hum:");
      lcd.setCursor(12,0);
      imprimirVariable(minH);
      lcd.setCursor(0, 1);
      lcd.print(">D hum:");
      lcd.setCursor(12,1);
      imprimirVariable(dH);
      break;       
    case 7:
      menu = 6;
      break;
  }
}

void updateValues(){
  
}

void executeAction() {
  switch (menu) {
    case 1:
      action1();
      break;
    case 2:
      action2();
      break;
    case 3:
      action3();
      break;
    case 4:
      action4();
      break;
    case 5:
      action5();
      break;
    case 6:
      action6();
      break;            
  }
}

void action1() {
    lcd.clear();
    lcd.print(" Max Temp:");
    imprimirFlechita(0);
    imprimirVariable(maxT);
    lcd.setCursor(0, 1);
    lcd.print(" Min Temp:");
    lcd.setCursor(12,1);
    imprimirVariable(minT);
    modificarYMostrar(&maxT, -10, 30,0);

}

void action2() {
    lcd.clear();
    lcd.print(" Max Temp:");
    lcd.setCursor(12,0);
    imprimirVariable(maxT);
    lcd.setCursor(0, 1);
    lcd.print(" Min Temp:");
    imprimirFlechita(1);
    imprimirVariable(minT);
    modificarYMostrar(&minT,-10,30,1);

}
void action3() {
    lcd.clear();
    lcd.print(" D Temp:");
    imprimirFlechita(0);
    imprimirVariable(dT);
    lcd.setCursor(0, 1);
    lcd.print(" Max hum:");
    lcd.setCursor(12,1);
    imprimirVariable(maxH);
    modificarYMostrar(&dT,0,15,0);
}
void action4() {
  lcd.clear();
  lcd.print(" D Temp:");
  lcd.setCursor(12,0);
  imprimirVariable(dT);
  lcd.setCursor(0, 1);
  lcd.print(" Max hum:");
  imprimirFlechita(1);
  imprimirVariable(maxH);
  modificarYMostrar(&maxH,0,100,1);
}
void action5() {
  lcd.clear();
  lcd.print(" Min hum:");
  imprimirFlechita(0);
  imprimirVariable(minH);
  lcd.setCursor(0, 1);
  lcd.print(" D hum:");
  lcd.setCursor(12,1);
  imprimirVariable(dH);
  modificarYMostrar(&minH,0,100,0);
}
void action6() {
  lcd.clear();
  lcd.print(" Min hum:");
  lcd.setCursor(12,0);
  imprimirVariable(minH);
  lcd.setCursor(0, 1);
  lcd.print(" D hum:");
  imprimirFlechita(1);
  imprimirVariable(dH);
  modificarYMostrar(&dH,0,20,1);
}
void modificarYMostrar(int* variable, int min, int max, int nivel){
    while (botonEncoder() == 0){
    modificarVariable(variable, min , max);
    lcd.setCursor(12,nivel);
    imprimirVariable(*variable);
  }
}

void imprimirFlechita(int nivel){
    lcd.setCursor(11,nivel);
    lcd.print(">");
}

void imprimirVariable(int variable){
    if(variable<10 && variable>=0)lcd.print(" ");
    if(variable>-10 && variable<0){
      lcd.print(" ");
      lcd.print(variable);
      lcd.print(" ");
    }
    if(variable<=-10)lcd.print(variable);
    else{
      lcd.print(" ");
      lcd.print(variable);
      lcd.print(" ");
    }
}

int dirEncoder(){//devuelve 1 si gira para la derecha, y -1 si para la izq
  encPos += encoder.getValue();

  if (encPos != oldEncPos) {
    if((oldEncPos - encPos)<0){
      return 1;  //der    
    }
    if((oldEncPos - encPos)>0){
      return -1; //izq
    }
    oldEncPos = encPos;

  }
}

void inicializar(){
  lcd.print("Inicializando");
  delay(500);
  lcd.print(".");
  delay(500);
  lcd.print(".");
  delay(500);
  lcd.print(".");
  delay(500);
}

void modificarVariable(int* variable, int min , int max){
    encPos += encoder.getValue();

  if (encPos != oldEncPos) {
    if((oldEncPos - encPos)<0){
      *variable = min(*variable + 1,max);  //der    
    }
    if((oldEncPos - encPos)>0){
      *variable = max(*variable - 1, min); //izq
    }
    oldEncPos = encPos;

  }

  
}

int botonEncoder(){
    buttonState = encoder.getButton();

  if (buttonState != 0) {

    switch (buttonState) {
      case ClickEncoder::Open:          //0
        break;

      case ClickEncoder::Closed:        //1
        break;

      case ClickEncoder::Pressed:       //2
        break;

      case ClickEncoder::Held:          //3
        return 2;
        break;

      case ClickEncoder::Released:      //4
        break;

      case ClickEncoder::Clicked:       //5
        return 1;  
        break;

      case ClickEncoder::DoubleClicked: //6
        return 6;
        break;
    }
  }
}
