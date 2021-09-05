#include <LiquidCrystal_I2C.h>
#include <ClickEncoder.h>
#include <TimerOne.h>
#include <DHT.h>
#include <DHT_U.h>
#include <EEPROM.h>


int16_t oldEncPos, encPos;
uint8_t buttonState;

#define pinA A2 //CLK
#define pinB A1 //DW
#define pinSw A0 //switch
#define STEPS 4
#define dhtPin 7
#define boton 6
int cooler = 8;
int heater = 9;
int humidificador = 10;

int estadoCooler = LOW;
int estadoHeater = LOW;
int estadoHumidificador = LOW;

float TEMP;
float HUM;
int menu = 1;
int tIdeal = 25;//Max temperatura
int dT = 2; // histeresis temperatura(delta T)
int hIdeal = 70; //Max humedad
int dH = 2; // histeresis humedad(delta H)
int menuConfig = 0;
unsigned long previousMillis = 0;
const long interval = 1000;


LiquidCrystal_I2C lcd(0x27,16,2);

ClickEncoder encoder(pinA, pinB, pinSw, STEPS);

DHT dht(dhtPin,DHT22);

byte onCharacter[] = {
  B00000,
  B00100,
  B01110,
  B10101,
  B10001,
  B10001,
  B01110,
  B00000
};

byte offCharacter[] = {
  B00000,
  B00000,
  B01110,
  B10001,
  B10001,
  B10001,
  B01110,
  B00000
};

byte gota[] = {
  B00100,
  B01010,
  B01010,
  B10001,
  B11111,
  B11111,
  B01110,
  B00000
};

byte termometro[] = {
  B00100,
  B01010,
  B01010,
  B01110,
  B01110,
  B11111,
  B11111,
  B01110
};


void setup() {
    lcd.init();
    lcd.backlight();
    lcd.createChar(1,onCharacter);
    lcd.createChar(2,offCharacter);
    lcd.createChar(3,gota);
    lcd.createChar(4,termometro);
    
    Timer1.initialize(1000);
    Timer1.attachInterrupt(timerIsr);
    
    pinMode(cooler,OUTPUT);
    pinMode(heater,OUTPUT);
    pinMode(humidificador,OUTPUT);
    pinMode(boton, INPUT_PULLUP);

    cargarVariables();  
    
    encoder.setAccelerationEnabled(true);
    
    dht.begin();


    lcd.clear();
    oldEncPos = 0;
    inicializar();
    lcd.clear();
    

}

void loop() {
  showDht();
  if(botonEncoder() == 1){
    lcd.clear();
    configuracion();
    lcd.clear();
  }
  
      
}

void cargarVariables(){
  int ubi = 0;
  leer(ubi,tIdeal);
  ubi += sizeof(int);
  leer(ubi,dT);
  ubi += sizeof(int);
  leer(ubi,hIdeal);
  ubi += sizeof(int);
  leer(ubi,dH);
  ubi += sizeof(int);

  
}

void updateVariables(){
  int ubi = 0;
  guardar(ubi,tIdeal);
  ubi += sizeof(int);
  guardar(ubi,dT);
  ubi += sizeof(int);
  guardar(ubi,hIdeal);
  ubi += sizeof(int);
  guardar(ubi,dH);
  ubi += sizeof(int);

  
}


void timerIsr() {
  encoder.service();
}


void configuracion(){
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
      updateVariables();
      quieroSeguir = false;
      while(!digitalRead(boton));
    }
  }
}

void showDht(){

    unsigned long currentMillis = millis();
    if(currentMillis - previousMillis >= interval){ //si paso el intervalo vuelve a medir
      TEMP = dht.readTemperature();
      HUM = dht.readHumidity();
      lcd.setCursor(0,0);
      lcd.print(char(4));
      imprimirVariable((int)TEMP);
      lcd.setCursor(5,0);
      lcd.print("Ht");
      imprimirEstado2(estadoHeater,6,1);
      imprimirBarra(8);
      lcd.setCursor(0,1);
      lcd.print(char(3));
      lcd.print("%");
      lcd.print(HUM,0);
      lcd.setCursor(10,0);
      lcd.print("C");
      imprimirEstado2(estadoCooler,10,1);
      imprimirBarra(12);
      lcd.setCursor(13,0);
      lcd.print("Hum");
      imprimirEstado2(estadoHumidificador,14,1);
      
      validarDatos(TEMP, HUM);    
    }
}

void imprimirBarra(int pos){
      lcd.setCursor(pos,0);
      lcd.print("|");
      lcd.setCursor(pos,1);
      lcd.print("|");
}

void validarDatos(float temp, float hum){

  controlarTemperatura(temp);
  controlarHumedad(hum);

}

void controlarTemperatura(float temp){ //TODO cuando llega a la tIdeal, bajar la temperatura hasta tIdeal - dT
  if(temp >= tIdeal + dT){//Si la temp es mayor que la ideal mas la histeresis, prendo el cooler
    encenderCooler();
  }
  else if(temp <= tIdeal - dT){
    encenderHeater();
  }
  else if(temp == tIdeal){//Si llegue a la temperatura indicada, apago todo
    apagarHeater();
    apagarCooler();
  }
  
  
}

void controlarHumedad(float hum){
  if((int)hum >= hIdeal + dH){//Si la temp es mayor que la ideal mas la histeresis, prendo el cooler
    encenderHumidificador();
  }
  else if((int)hum <= hIdeal - dH){
    //nose
  }
  else if((int)hum == hIdeal){//Si llegue a la temperatura indicada, apago todo
    apagarHumidificador();
  }
  
  
}

void encenderCooler(){
  digitalWrite(cooler,HIGH);
  estadoCooler = HIGH;
}
void encenderHeater(){
  digitalWrite(heater,HIGH);
  estadoHeater = HIGH;  
}
void encenderHumidificador(){
  digitalWrite(humidificador, HIGH);
  estadoHumidificador = HIGH;  
}
void apagarCooler(){
  digitalWrite(cooler,LOW);
  estadoCooler = LOW;  
}
void apagarHeater(){
  digitalWrite(heater,LOW);
  estadoHeater = LOW;  
}
void apagarHumidificador(){
  digitalWrite(humidificador,LOW);
  estadoHumidificador = LOW;  
}

void updateMenu() {
  switch (menu) {
    case 0:
      menu = 1;
      break;
    case 1:
      lcd.clear();
      lcd.print(">T ideal:");
      lcd.setCursor(12,0);
      imprimirVariable(tIdeal);
      lcd.setCursor(0, 1);
      lcd.print(" H ideal:");
      lcd.setCursor(12,1);
      imprimirVariable(hIdeal);
      break;
    case 2:
      lcd.clear();
      lcd.print(" T ideal:");
      lcd.setCursor(12,0);
      imprimirVariable(tIdeal);
      lcd.setCursor(0, 1);
      lcd.print(">H ideal:");
      lcd.setCursor(12,1);
      imprimirVariable(hIdeal);
      break;
    case 3:
      lcd.clear();
      lcd.print(">D Temp:");
      lcd.setCursor(12,0);
      imprimirVariable(dT);
      lcd.setCursor(0, 1);
      lcd.print(" D hum:");
      lcd.setCursor(12,1);
      imprimirVariable(dH);
      break;
    case 4:
      lcd.clear();
      lcd.print(" D Temp:");
      lcd.setCursor(12,0);
      imprimirVariable(dT);
      lcd.setCursor(0, 1);
      lcd.print(">D hum:");
      lcd.setCursor(12,1);
      imprimirVariable(dH);
      break;
    case 5:
      lcd.clear();
      lcd.print(">Cooler:");
      imprimirEstado(estadoCooler,13,0);
      lcd.setCursor(0, 1);
      lcd.print(" Heater:");
      imprimirEstado(estadoHeater,13,1);
      break;
    case 6:
      lcd.clear();
      lcd.print(" Cooler:");
      imprimirEstado(estadoCooler,13,0);
      lcd.setCursor(0, 1);
      lcd.print(">Heater:");
      lcd.setCursor(12,1);
      imprimirEstado(estadoHeater,13, 1);
      break;
    case 7:
      lcd.clear();
      lcd.print(">Humidif:");
      imprimirEstado(estadoHumidificador,13,0);
      lcd.setCursor(0, 1);
      lcd.print(" Reset");
      break;
    case 8:
      lcd.clear();
      lcd.print(" Humidif:");
      imprimirEstado(estadoHumidificador,13,0);
      lcd.setCursor(0, 1);
      lcd.print(">Reset");
      break;                                 
    case 9:
      menu = 8;
      break;
  }
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
    case 7:
      action7();
      break;                              
    case 8:
      action8();
      break;        
  }
}

void action1() {
    
    lcd.clear();
    lcd.print(" T ideal:");
    imprimirFlechita(0);
    imprimirVariable(tIdeal);
    lcd.setCursor(0, 1);
    lcd.print(" H ideal:");
    lcd.setCursor(12,1);
    imprimirVariable(hIdeal);
    modificarYMostrar(&tIdeal, -10, 30,0);

}

void action2() {
    lcd.clear();
    lcd.print(" T ideal:");
    lcd.setCursor(12,0);
    imprimirVariable(tIdeal);
    lcd.setCursor(0, 1);
    lcd.print(" H ideal:");
    imprimirFlechita(1);
    imprimirVariable(hIdeal);
    modificarYMostrar(&hIdeal,-10,30,1);

}
void action3() {
    lcd.clear();
    lcd.print(" D Temp:");
    imprimirFlechita(0);
    imprimirVariable(dT);
    lcd.setCursor(0, 1);
    lcd.print(" D hum:");
    lcd.setCursor(12,1);
    imprimirVariable(dH);
    modificarYMostrar(&dT,0,15,0);
}
void action4() {
  lcd.clear();
  lcd.print(" D Temp:");
  lcd.setCursor(12,0);
  imprimirVariable(dT);
  lcd.setCursor(0, 1);
  lcd.print(" D hum:");
  imprimirFlechita(1);
  imprimirVariable(dH);
  modificarYMostrar(&dH,0,100,1);
}

void action5() {
  lcd.clear();
  lcd.print(" Cooler:");
  imprimirFlechita(0);
  imprimirEstado(estadoCooler,13,0);
  lcd.setCursor(0, 1);
  lcd.print(" Heater:");
  imprimirEstado(estadoHeater,13,1);
  modificarYMostrarEstado(&estadoCooler,0);

}

void action6() {
  lcd.clear();
  lcd.print(" Cooler:");
  imprimirEstado(estadoCooler,13,0);
  lcd.setCursor(0, 1);
  lcd.print(" Heater:");
  imprimirFlechita(1);
  lcd.setCursor(12,1);
  imprimirEstado(estadoHeater,13, 1);
  modificarYMostrarEstado(&estadoHeater,1);

}

void action7() {
  lcd.clear();
  lcd.print(" Humidif:");
  imprimirFlechita(0);
  imprimirEstado(estadoHumidificador,13,0);
  lcd.setCursor(0, 1);
  lcd.print(" Reset");
  modificarYMostrarEstado(&estadoHumidificador,0);

}



void action8(){
  lcd.clear();
  lcd.print(" Esta seguro?");
  lcd.setCursor(3,1);
  lcd.print("Si");
  lcd.setCursor(12,1);
  lcd.print("No");
  imprimirFlechita2(11,1);
  elegirOpcion();
}

void elegirOpcion(){
  int seleccion = false;
  while (botonEncoder() == 0){
      modificarFlechita(&seleccion);
  }
  if(seleccion == true){
    resetear();
  }
}

void resetear(){
  lcd.clear();
  lcd.print("Reseteando...");
  
  tIdeal = 25;//Max temperatura
  dT = 2; // histeresis temperatura(delta T)
  hIdeal = 70; //Max humedad
  dH = 2; // histeresis humedad(delta H)

  updateVariables();
  delay(500);
}

void modificarFlechita(int* estado){
  encPos += encoder.getValue();

  if (encPos != oldEncPos) {
    if((oldEncPos - encPos)<0 ){
      imprimirFlechita2(11,1);  //der
      borrar(2,1);
      *estado = false;    
    }
    if((oldEncPos - encPos)>0){
      imprimirFlechita2(2,1); //izq
      borrar(11,1);
      *estado = true;
    }
    oldEncPos = encPos;

  } 
}

void borrar(int pos, int nivel){
  lcd.setCursor(pos,nivel);
  lcd.print(" ");
}

void modificarEstado(int* estado){
  encPos += encoder.getValue();

  if (encPos != oldEncPos) {
    if((oldEncPos - encPos)<0 && *estado == LOW){
      *estado = HIGH;  //der    
    }
    if((oldEncPos - encPos)>0 && *estado == HIGH){
      *estado = LOW; //izq
    }
    oldEncPos = encPos;

  } 
}

void modificarYMostrarEstado(int* variable, int nivel){
    while (botonEncoder() == 0){
      modificarEstado(variable);
      imprimirEstado(*variable,13, nivel);
  }
}


void modificarYMostrar(int* variable, int min, int max, int nivel){
    while (botonEncoder() == 0){
    modificarVariable(variable, min , max);
    lcd.setCursor(12,nivel);
    imprimirVariable(*variable);
  }
}

void imprimirFlechita(int nivel){
    imprimirFlechita2(11,nivel);
}
void imprimirFlechita2(int pos, int nivel){
    lcd.setCursor(pos,nivel);
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

void imprimirEstado(int estado,int pos, int nivel){
  lcd.setCursor(pos,nivel);
  if(estado == LOW){
    lcd.print("OFF");
  }
  else if(estado == HIGH){
    lcd.print("ON");
    lcd.print(" ");
  }
  
}

void imprimirEstado2(int estado,int pos, int nivel){
  lcd.setCursor(pos,nivel);
  if(estado == LOW){
    lcd.print(char(2));
  }
  else if(estado == HIGH){
    lcd.print(char(1));
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

template <class T> int guardar(int ee, const T& value)
{
    const byte* p = (const byte*)(const void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
          EEPROM.write(ee++, *p++);
    return i;
}

template <class T> int leer(int ee, T& value)
{
    byte* p = (byte*)(void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
          *p++ = EEPROM.read(ee++);
    return i;
}
