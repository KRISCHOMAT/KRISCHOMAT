////////**********\\\\\\\\
/////// Matrix Seq \\\\\\\\
 //////**  V 1.0  **\\\\\\
 // BY CHRISTIAN GROTHE \\
   // www.kryhse.com \\
////////**********\\\\\\\\
//**********************\\

#include <Bounce2.h>
//#include "Mux.h"
//using namespace admux; 

const uint8_t NUM_BUTTONSX = 3;
const uint8_t NUM_BUTTONSY = 3;
const uint8_t NUM_XY = 12;
const uint8_t NUM_BITOUT = 3;
const uint8_t NUM_BITS = 3;

bool xyStates[NUM_XY] = {true, false, false, false, true, false, false, false, true};

const uint8_t LEDOuts[NUM_XY] = {1,2,3,4,5,6,7,8,9};
const uint8_t BUTTONSX[NUM_BUTTONSX] = {5, 6, 7};
const uint8_t BUTTONSY[NUM_BUTTONSY] = {8, 9, 10};

int BitOut[NUM_BITOUT] = {2,3,4};

Bounce * buttonsX = new Bounce[NUM_BUTTONSX];
Bounce * buttonsY = new Bounce[NUM_BUTTONSY];
Bounce2::Button b = Bounce2::Button();

uint8_t bits[NUM_BITS] = {0, 0, 0};
bool sumofbits = 0;

uint8_t counter = 0;
uint8_t countmax = 9;

const uint8_t trigger = 13;

bool triggerState = 0;
bool triggering = 0;
bool triggeringold = 0;

// autotrigger
unsigned long previousMillis = 0;


void setup() {

  Serial.begin(9600);

  b.attach (trigger, INPUT_PULLUP);
  b.interval(5);
  b.setPressedState(LOW);

    for (int i = 0; i< NUM_BITOUT ; i++){ 
      pinMode(BitOut[i], OUTPUT);
    }
    
    for (int i = 0; i< NUM_BUTTONSX; i++){
      buttonsX[i].attach(BUTTONSX[i], INPUT_PULLUP);
      buttonsX[i].interval(25);
    }
    
    for (int i = 0; i< NUM_BUTTONSY; i++){
      buttonsY[i].attach(BUTTONSY[i], INPUT_PULLUP);
      buttonsY[i].interval(25);
    }
}

void loop() {

// update buttons
    b.update();
    
    for (int i = 0; i < NUM_BUTTONSX; i++){
    buttonsX[i].update();
  }
  
    for (int i = 0; i < NUM_BUTTONSY; i++){
    buttonsY[i].update();
  }

// trigger & counter
    triggering = autotrigger(300);

  if (triggering != triggeringold){
    counter += 1;
    triggeringold = triggering;
    if (counter == countmax){
      counter = 1;
    }  
  }

  Serial.println(counter);  
  for (int i = 0; i < NUM_BITS; i++){
    bits[i] = bitRead(counter, i);
  }

// routing
  for (int i = 0; i < NUM_BUTTONSY; i++){  
    matrix(0,i,i);
    matrix(1,i,i+3);
    matrix(2,i,i+6);
  }

// bit output
   digitalWrite(BitOut[0], sumofbits_function(0));
   digitalWrite(BitOut[1], sumofbits_function(3));
   digitalWrite(BitOut[2], sumofbits_function(6));
      
// led outs matrix
    for (int i =0; i < NUM_XY; i++){
  //digitalWrite(LEDOuts[i], xyStates[i]);
    }
  
}


bool matrix(int x,int y, int xy){
  
  int xValue = buttonsX[x].read();
  int yValue = buttonsY[y].read();

  if (buttonsX[x].fell() && yValue == LOW || xValue == LOW && buttonsY[y].fell()) {
    xyStates[xy] = !xyStates[xy];
  } 
}


bool sumofbits_function (int i){

  if ((xyStates[0 + i] && xyStates[1 + i] && xyStates[2 + i]) && (bits[0] || bits[1] || bits[2])){
    sumofbits = 1;
  } else if ((xyStates[0 + i] && xyStates[1 + i]) && (bits[0] || bits[1])){
    sumofbits = 1;
  } else if ((xyStates[1 + i] && xyStates[2 + i]) && (bits[1] || bits[2])){
    sumofbits = 1;
  } else if ((xyStates[0 + i] && xyStates[2 + i]) && (bits[0] || bits[2])){
    sumofbits = 1;
  } else if ((xyStates[0 + i]) && (bits[0])){
    sumofbits = 1;
  } else if ((xyStates[1 + i]) && (bits[1])){
    sumofbits = 1;
  } else if ((xyStates[2 + i]) && (bits[2])){
    sumofbits = 1;
  } else {
    sumofbits = 0;
  } 
    return sumofbits; 
  }


bool autotrigger (int interval){
  
        unsigned long currentMillis = millis();
        
  if (currentMillis - previousMillis >=interval){
    previousMillis = currentMillis;
    
    if (triggerState == 0) {
    triggerState = 1;
  } else {
    triggerState = 0;
  }
  }
  return triggerState;
}
