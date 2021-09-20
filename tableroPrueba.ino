#include <FastLED.h>
#include <IRremote.h>
#define countof(a) (sizeof(a) / sizeof(a[0]))
#define NUM_LEDS 86
#define DATA_PIN 6
#define MILLI_AMPS 2400

const byte IR_RECEIVE_PIN = 11;

//Declaracion de variables globales
CRGB LEDs[NUM_LEDS];
byte r_val = 255;
byte g_val = 0;
byte b_val = 0;
byte brightness = 255;
CRGB color = CRGB(r_val, g_val, b_val);
unsigned long countdownMilliSeconds;
unsigned long endCountDownMillis;
unsigned long contadorVal;
unsigned long restMS;
bool pause = false;
bool primeraEjecucion = true;
bool countdownOn = false;
CRGB countdownColor = CRGB::Red;
CRGB alternateColor = CRGB::Black;
byte scoreboardLeft = 0;
byte scoreboardRight = 0;
CRGB scoreboardColorLeft = CRGB::Green;
CRGB scoreboardColorRight = CRGB::Blue;
bool scoreboardOn = false;
//Matriz que alamacena los numeros como string 
String numbers[] = {
  "111111111111111111000",  // [0] 0
  "111000000000000111000",  // [1] 1
  "000111111000111111111",  // [2] 2
  "111111000000111111111",  // [3] 3
  "111000000111000111111",  // [4] 4
  "111111000111111000111",  // [5] 5
  "111111111111111000111",  // [6] 6
  "111000000000111111000",  // [7] 7
  "111111111111111111111",  // [8] 8
  "111111000111111111111",  // [9] 9
  "000000000000000000000",  // [10] off
};

void setup() {
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(LEDs, NUM_LEDS);
  Serial.begin(115200);
  delay(200);    
  FastLED.setDither(false);
  FastLED.setCorrection(TypicalLEDStrip);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, MILLI_AMPS);
  fill_solid(LEDs, NUM_LEDS, CRGB::Black);
  displayVal(0);
  FastLED.show();
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
}

void loop() {
  //en la primera ejecucion guardo las variables, esto luego tiene que pasar a activarse cuando el usuario lo desee  
  
  //Comandos del control infrarojo
  if (IrReceiver.decode()){
    Serial.println(IrReceiver.decodedIRData.command, HEX);
    Serial.println(IrReceiver.decodedIRData.command);
    // Iniciar marcador de puntos
    if(IrReceiver.decodedIRData.command == 69){
      scoreboardOn = true;
      delay(500);
      IrReceiver.resume();
    }
    //sumar un punto equipo izq
    if(IrReceiver.decodedIRData.command == 71){
      scoreboardRight = scoreboardRight + 1;
      updateScoreboard();      
      FastLED.show();
      delay(500);
      IrReceiver.resume();
    }
    //restar un punto equipo izq
    if(IrReceiver.decodedIRData.command == 67){
      if(scoreboardRight > 0){
        scoreboardRight = scoreboardRight - 1;
      }
      updateScoreboard();      
      FastLED.show();
      delay(500);
      IrReceiver.resume();
    }
    //sumar un punto equipo derecha
    if(IrReceiver.decodedIRData.command == 70){
      scoreboardLeft = scoreboardLeft + 1;
      updateScoreboard();      
      FastLED.show();
      delay(500);
      IrReceiver.resume();
    }
    //restar un punto equipo derecha
    if(IrReceiver.decodedIRData.command == 64){
      if(scoreboardLeft > 0){
        scoreboardLeft = scoreboardLeft - 1;
      }
      updateScoreboard();
      FastLED.show();
      IrReceiver.resume();  
      delay(500);
    }
    // Iniciar cronometro
    if(IrReceiver.decodedIRData.command == 7){
      countdownOn = true;
      setCountdown(contadorVal);
      delay(500);
      IrReceiver.resume();
    }
    //Pausa
    if(IrReceiver.decodedIRData.command == 0){
      countdownOn = false;
      pause = true;
    }
    //Agregar 1 minuto al contador
    if(IrReceiver.decodedIRData.command == 9){
      if(pause){
        restMS = restMS + 60000;
        displayVal(restMS);
      }else {
        contadorVal = contadorVal + 60000;
        displayVal(contadorVal);  
      }
      FastLED.show();
      delay(300);
    }
    //Agregar 10 minutos al contador
    if(IrReceiver.decodedIRData.command == 21){
      if(pause){
        restMS = restMS + 600000;
        displayVal(restMS);
      }else {
        contadorVal = contadorVal + 600000;
        displayVal(contadorVal);  
      }    
      FastLED.show();
      delay(300);
    }
    //Resetear el contador a cero
    if(IrReceiver.decodedIRData.command == 68){
      contadorVal = 0;
      restMS = 0;
      countdownMilliSeconds = 0;
      endCountDownMillis = 0;
      displayVal(contadorVal);      
      FastLED.show();
      delay(300);
    }
    IrReceiver.resume();
  }
  //Ejecucion de funciones
  //iniciar cronometro en valor asignado en contadorVal  
  if(countdownOn) {
    updateCountdown();
    FastLED.show();
  }
  if(scoreboardOn){
    updateScoreboard();
    FastLED.show();
  }
}

void displayNumber(byte number, byte segment, CRGB color) {
  /*
   * 
      __ __ __        __ __ __          __ __ __        12 13 14  
    __        __    __        __      __        __    11        15
    __        __    __        __      __        __    10        16
    __        __    __        __  42  __        __    _9        17
      __ __ __        __ __ __          __ __ __        20 19 18  
    __        65    __        44  43  __        21    _8        _0
    __        __    __        __      __        __    _7        _1
    __        __    __        __      __        __    _6        _2
      __ __ __       __ __ __           __ __ __       _5 _4 _3   
   */
 
  // segment from left to right: 3, 2, 1, 0
  byte startindex = 0;
  switch (segment) {
    case 0:
      startindex = 0;
      break;
    case 1:
      startindex = 21;
      break;
    case 2:
      startindex = 44;
      break;
    case 3:
      startindex = 65;
      break;    
  }

  String num = numbers[number];
  for (byte i=0; i<21; i++){
    LEDs[i + startindex] = (char(num[i]) == '1') ? color : alternateColor;
  }
}

void showDots(CRGB color) {
  LEDs[42] = color;
  LEDs[43] = color;
}

void hideDots(CRGB color) {
  LEDs[42] = CRGB::Black;
  LEDs[43] = CRGB::Black;
}

void setCountdown(unsigned long countdownVal){
  countdownMilliSeconds = countdownVal;
//  countdownMilliSeconds = 360000;
  //5s+10s = 15s
  //millis() == 15s -> termina la ejecucion del cronometro
  endCountDownMillis = millis() + countdownMilliSeconds;
}

void updateCountdown() {

  if(pause){
    endCountDownMillis = millis() + restMS;
    pause = false;
  }

  if (countdownMilliSeconds == 0 && endCountDownMillis == 0) 
    return;
  //tiempo restante en ms = 15s - 7s = 8s
  //tiempo restante en ms = 15s - 8s = 7s
  //tiempo restante en ms = 15s - 9s = 6s
  //tiempo restante en ms = 15s - 15s = 0s
  unsigned long restMillis = endCountDownMillis - millis();
  restMS = restMillis;
  unsigned long hours   = ((restMillis / 1000) / 60) / 60;
  unsigned long minutes = (restMillis / 1000) / 60;
  unsigned long seconds = restMillis / 1000;
  int remSeconds = seconds - (minutes * 60);
  int remMinutes = minutes - (hours * 60); 
  
//  Serial.print(restMillis);
//  Serial.print(" ");
//  Serial.print(hours);
//  Serial.print(" ");
//  Serial.print(minutes);
//  Serial.print(" ");
//  Serial.print(seconds);
//  Serial.print(" | ");
//  Serial.print(remMinutes);
//  Serial.print(" ");
//  Serial.println(remSeconds);

  if(remSeconds%2 == 0){
    showDots(color);
  } else {
    hideDots(color);
  }

  byte h1 = hours / 10;
  byte h2 = hours % 10;
  byte m1 = minutes / 10;
  byte m2 = minutes % 10;  
  byte s1 = remSeconds / 10;
  byte s2 = remSeconds % 10;

//  if (hours > 0) {
//    // hh:mm
//    displayNumber(h1,3,color); 
//    displayNumber(h2,2,color);
//    displayNumber(m1,1,color);
//    displayNumber(m2,0,color);  
//  } else {
    // mm:ss   
    displayNumber(m1,3,color);
    displayNumber(m2,2,color);
    displayNumber(s1,1,color);
    displayNumber(s2,0,color);  
//  }

  if (hours <= 0 && remMinutes <= 0 && remSeconds <= 0) {
    Serial.println("Countdown timer ended.");
    //endCountdown();
    countdownOn = false;
    countdownMilliSeconds = 0;
    endCountDownMillis = 0;
    return;
  }  
}

void displayVal(unsigned long val) {
  unsigned long restMillis = val;
  unsigned long hours   = ((restMillis / 1000) / 60) / 60;
  unsigned long minutes = (restMillis / 1000) / 60;
  unsigned long seconds = restMillis / 1000;
  int remSeconds = seconds - (minutes * 60);
  int remMinutes = minutes - (hours * 60); 
  
  showDots(color);

  byte h1 = hours / 10;
  byte h2 = hours % 10;
  byte m1 = minutes / 10;
  byte m2 = minutes % 10;  
  byte s1 = remSeconds / 10;
  byte s2 = remSeconds % 10;

//  if (hours > 0) {
//    // hh:mm
//    displayNumber(h1,3,color); 
//    displayNumber(h2,2,color);
//    displayNumber(m1,1,color);
//    displayNumber(m2,0,color);  
//  } else {
    // mm:ss   
    displayNumber(m1,3,color);
    displayNumber(m2,2,color);
    displayNumber(s1,1,color);
    displayNumber(s2,0,color);  
//  }
}

void updateScoreboard() {
  byte sl1 = scoreboardLeft / 10;
  byte sl2 = scoreboardLeft % 10;
  byte sr1 = scoreboardRight / 10;
  byte sr2 = scoreboardRight % 10;

  displayNumber(sl1,3,scoreboardColorLeft);
  displayNumber(sl2,2,scoreboardColorLeft);
  displayNumber(sr1,1,scoreboardColorRight);
  displayNumber(sr2,0,scoreboardColorRight);
  showDots(color);
  scoreboardOn = false;
}
