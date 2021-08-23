#include <FastLED.h>
#define countof(a) (sizeof(a) / sizeof(a[0]))
#define NUM_LEDS 21
#define DATA_PIN 6
#define MILLI_AMPS 2400

CRGB LEDs[NUM_LEDS];

// Settings
unsigned long prevTime = 0;
byte r_val = 255;
byte g_val = 0;
byte b_val = 0;
bool dotsOn = true;
byte brightness = 255;
byte clockMode = 0;                           // Clock modes: 0=Clock, 1=Countdown, 2=Temperature, 3=Scoreboard
unsigned long countdownMilliSeconds;
unsigned long endCountDownMillis;           
CRGB countdownColor = CRGB::Red;
byte scoreboardLeft = 0;
byte scoreboardRight = 0;
CRGB scoreboardColorLeft = CRGB::Green;
CRGB scoreboardColorRight = CRGB::Red;
CRGB alternateColor = CRGB::Black; 
//String numbers[] = {
//  "000111111111111111111",  // [0] 0
//  "000111000000000000111",  // [1] 1
//  "11111111100011111100",  // [2] 2
//  "111111111000000111111",  // [3] 3
//  "111111000111000000111",  // [4] 4
//  "111000111111000111111",  // [5] 5
//  "111000111111111111111",  // [6] 6
//  "000111111000000000111",  // [7] 7
//  "111111111111111111111",  // [8] 8
//  "111111111111000111111",  // [9] 9
//  "000000000000000000000",  // [10] off
//};
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
  Serial.begin(9600);  
  //FastLED.setDither(false);
  //FastLED.setCorrection(TypicalLEDStrip);
//  FastLED.setMaxPowerInVoltsAndMilliamps(5, MILLI_AMPS);
//  fill_solid(LEDs, NUM_LEDS, CRGB::Black);
//  FastLED.show();
  
    countdownMilliSeconds = 8000;     
    byte cd_r_val = 255;
    byte cd_g_val = 0;
    byte cd_b_val = 0;
//    digitalWrite(COUNTDOWN_OUTPUT, LOW);
    countdownColor = CRGB(cd_r_val, cd_g_val, cd_b_val); 
    endCountDownMillis = millis() + countdownMilliSeconds;
    allBlank();
}

void loop() {
  CRGB color = CRGB(r_val, g_val, b_val);
  displayNumber(9,0,color);
  FastLED.show();
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

//countdown
void updateCountdown() {

  if (countdownMilliSeconds == 0 && endCountDownMillis == 0) 
    return;
    
  unsigned long restMillis = endCountDownMillis - millis();
  unsigned long hours   = ((restMillis / 1000) / 60) / 60;
  unsigned long minutes = (restMillis / 1000) / 60;
  unsigned long seconds = restMillis / 1000;
  int remSeconds = seconds - (minutes * 60);
  int remMinutes = minutes - (hours * 60); 
  
  Serial.print(restMillis);
  Serial.print(" ");
  Serial.print(hours);
  Serial.print(" ");
  Serial.print(minutes);
  Serial.print(" ");
  Serial.print(seconds);
  Serial.print(" | ");
  Serial.print(remMinutes);
  Serial.print(" ");
  Serial.println(remSeconds);

  byte h1 = hours / 10;
  byte h2 = hours % 10;
  byte m1 = remMinutes / 10;
  byte m2 = remMinutes % 10;  
  byte s1 = remSeconds / 10;
  byte s2 = remSeconds % 10;

  CRGB color = countdownColor;
  if (restMillis <= 60000) {
    color = CRGB::Red;
  }

  if (hours > 0) {
    // hh:mm
    displayNumber(h1,3,color); 
    displayNumber(h2,2,color);
    displayNumber(m1,1,color);
    displayNumber(m2,0,color);  
  } else {
    // mm:ss   
    displayNumber(m1,3,color);
    displayNumber(m2,2,color);
    displayNumber(s1,1,color);
    displayNumber(s2,0,color);  
  }

//  displayDots(color);  

  if (hours <= 0 && remMinutes <= 0 && remSeconds <= 0) {
    Serial.println("Countdown timer ended.");
    //endCountdown();
    countdownMilliSeconds = 0;
    endCountDownMillis = 0;
//    digitalWrite(COUNTDOWN_OUTPUT, HIGH);
    return;
  }  
}
//end countdown

void allBlank() {
  for (int i=0; i<NUM_LEDS; i++) {
    LEDs[i] = CRGB::Black;
  }
  FastLED.show();
}
