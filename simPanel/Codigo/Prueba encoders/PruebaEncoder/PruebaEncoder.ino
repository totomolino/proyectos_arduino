#include <Encoder.h>


Encoder myEnc(4,5);

int counter = -1;

void setup() {

  Serial.begin(9600);
 
}


long oldPosition = -999;

void loop() {
 checkEncoder();
  }

void checkEncoder(){
  int result = encDir();
  if(result != 2){
    if(result == 1){counter ++;}
    else counter --;
    Serial.println(counter);  
  }
}


int encDir() {
  long newPosition = myEnc.read();
  if (newPosition != oldPosition) {
    if(newPosition > oldPosition){
      oldPosition = newPosition;
      return 1;
      }
    else {
      oldPosition = newPosition;
      return 0;
    }   
  }
  return 2;
}
