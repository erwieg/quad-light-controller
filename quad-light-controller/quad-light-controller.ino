/*
  QuadLight Controller

  Controller for four RGB LED strips, color, pattern, brightness, etc.
  Board: Adafruit ItsyBitsy 5V
  Programmer: AVRISP mkII

  Current Build Status
  OK - Built in LED status on ItsyBitsy 5V
  OK - LED Array functionality using NeoPixelBus, N=30 Neopixel Strand
  OK - 3 modes selectable by first button, ALL, INDIVIDUAL, PATTERN
  OK - ALL & Individual potentiometer control
  OK - Pattern mode button selects pattern if in pattern mode
  OK - Dimness and Saturation buttons
  TBD - In ALL mode, control brightness and saturation with 2nd and 3rd POT
  TBD - Pattern variable control with pots

  created 18 Sept 2020
  Eric Wiegmann

*/

#include "FastLED.h"

#define LED_PIN  LED_BUILTIN  // LED on ItsyBitsy for status

//LED Definitions
#define DATA_1_PIN 10 //Set LED strip #1 data pin
#define DATA_2_PIN 9 //Set LED strip #2 data pin
#define DATA_3_PIN 7 //Set LED strip #3 data pin
#define DATA_4_PIN 5 //Set LED strip #4 data pin

#define LED_TYPE WS2811 //Set LED type
#define COLOR_ORDER GRB //Set LED calibration
#define NUM_LEDS 30 //Set LED strip length
#define BRIGHTNESS 100; //Set LED brightness

// Define the array of leds to set/clear LED data
CRGB leds1[NUM_LEDS];
CRGB leds2[NUM_LEDS];
CRGB leds3[NUM_LEDS];
CRGB leds4[NUM_LEDS];

#define BUTTON_1_PIN 12 // Strand Mode button, all mode, individual mode, pattern mode
#define BUTTON_2_PIN 11 // Pattern Select button
#define BUTTON_3_PIN 6 // Dimmer
#define BUTTON_4_PIN 4 // Saturation

#define POT_1_PIN A0 // Knob 1 pin
#define POT_2_PIN A1 // Knob 2 pin
#define POT_3_PIN A2 // Knob 3 pin
#define POT_4_PIN A3 // Knob 4 pin

// Initialize poteniometer color values from each pot as an array
uint8_t potValues_8bit[] = {0, 0, 0, 0};

// Moving average, each potentiometer uses a moving average to eliminate LED flicker from unstable pot readings
const int numReadings = 20;
int readIndex = 0;
int pot1Readings[numReadings] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int pot2Readings[numReadings] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int pot3Readings[numReadings] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int pot4Readings[numReadings] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int pot1Total = 0;
int pot2Total = 0;
int pot3Total = 0;
int pot4Total = 0;
int pot1Average = 0;
int pot2Average = 0;
int pot3Average = 0;
int pot4Average = 0;

// Initialize dimness and saturation values
uint8_t dimnessValue_8bit = 255;
uint8_t saturationValue_8bit = 255;

// Initialize the dimmer and saturation variables, initialize to full brightness and saturation
//boolean strandModeAll = true; // Determines LED mode, ALL or INDIVIDUAL control
int modeButtonCount = 2; // Determines what mode the quadlight will be in (2: All mode, 1: Individual mode, 0: Pattern mode)
int patternButtonCount = 4; // Determines what pattern is active, range 0-5
int dimnessButtonCount = 10; // Determines brightness of each strand, range 0-10, initialize to full brightness
int saturationButtonCount = 10; // Determines saturation of each strand, range 0-10, initialize to full saturation

int buttonDelay = 200; // Delay for button presses to eliminate multiple registers

//For FastLED Patterns (from DemoReel100 example)
uint8_t gHue = 0; // rotating "base color" used by many of the patterns


// the setup function runs once when you press reset or power the board
void setup() {
  
  Serial.begin(9600);

  // Tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, DATA_1_PIN, COLOR_ORDER>(leds1, NUM_LEDS);
  FastLED.addLeds<LED_TYPE, DATA_2_PIN, COLOR_ORDER>(leds2, NUM_LEDS);
  FastLED.addLeds<LED_TYPE, DATA_3_PIN, COLOR_ORDER>(leds3, NUM_LEDS);
  FastLED.addLeds<LED_TYPE, DATA_4_PIN, COLOR_ORDER>(leds4, NUM_LEDS);
  
  // Initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  // Initialize buttons and potentiometers as inputs
  pinMode(BUTTON_1_PIN, INPUT);
  pinMode(BUTTON_2_PIN, INPUT);
  pinMode(BUTTON_3_PIN, INPUT);
  pinMode(BUTTON_4_PIN, INPUT);
  pinMode(POT_1_PIN, INPUT);
  pinMode(POT_2_PIN, INPUT);
  pinMode(POT_3_PIN, INPUT);
  pinMode(POT_4_PIN, INPUT);
  
}


// the loop function runs over and over again forever
void loop() {

  // Turn on built-in LED to show OK status
  digitalWrite(LED_PIN, HIGH);

  // Read mode button value to set which light mode is active
  setMode();

  
  // 2: All strand control mode
  if (modeButtonCount == 2) {

    setDimness(); // Reads and sets current dimness value
    setSaturation(); // Reads and sets current saturation value
    //readPotentiometers();
    readPotentiometersAverage(); // Reads and sets color based on current pot value

    //Set all strands to color value of first potentiometer
    leds1On(potValues_8bit[0], saturationValue_8bit, dimnessValue_8bit);
    leds2On(potValues_8bit[0], saturationValue_8bit, dimnessValue_8bit);
    leds3On(potValues_8bit[0], saturationValue_8bit, dimnessValue_8bit);
    leds4On(potValues_8bit[0], saturationValue_8bit, dimnessValue_8bit);
  }
  
  // 1: Individual strand control mode
  if (modeButtonCount == 1) {

    setDimness(); // Reads and sets current dimness value
    setSaturation(); // Reads and sets current saturation value
    //readPotentiometers();
    readPotentiometersAverage(); // Reads and sets color based on current pot value
  
    leds1On(potValues_8bit[0], saturationValue_8bit, dimnessValue_8bit);
    leds2On(potValues_8bit[1], saturationValue_8bit, dimnessValue_8bit);
    leds3On(potValues_8bit[2], saturationValue_8bit, dimnessValue_8bit);
    leds4On(potValues_8bit[3], saturationValue_8bit, dimnessValue_8bit);
  }

  // 0: Pattern strand control mode
  if (modeButtonCount == 0) {
    setPattern();
    readPotentiometersAverage(); // Reads and sets color based on current pot value
    
    FastLED.show(); //Once pattern is set, show it
    EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  }
   

}






/*___________ADDITIONAL FUNCTIONS_____________*/

// Functions to set colors for each strand
// LED Strand 1 On function
void leds1On(int h, int s, int v) {
  for(int i=0; i<NUM_LEDS; i++) {
    leds1[i] = CHSV(h, s, v);
  }
  FastLED.show();
}

// LED Strand 2 On function
void leds2On(int h, int s, int v) {
  for(int i=0; i<NUM_LEDS; i++) {
    leds2[i] = CHSV(h, s, v);
  }
  FastLED.show();
}

// LED Strand 3 On function
void leds3On(int h, int s, int v) {
  for(int i=0; i<NUM_LEDS; i++) {
    leds3[i] = CHSV(h, s, v);
  }
  FastLED.show();
}

// LED Strand 4 On function
void leds4On(int h, int s, int v) {
  for(int i=0; i<NUM_LEDS; i++) {
    leds4[i] = CHSV(h, s, v);
  }
  FastLED.show();
}


// Function to read all potentiometers and convert value to 8-bit number
void readPotentiometers() {
    
    //Potentiometer Readings
    int pot1Voltage = analogRead(POT_1_PIN);
    int pot2Voltage = analogRead(POT_2_PIN);
    int pot3Voltage = analogRead(POT_3_PIN);
    int pot4Voltage = analogRead(POT_4_PIN);
    
    //Convert analog reading (range of 0-1000) to a 8-bit value (0.0-255.0), it will be a decimal
    float pot1Value = pot1Voltage * (255.0 / 1000.0);
    float pot2Value = pot2Voltage * (255.0 / 1000.0);
    float pot3Value = pot3Voltage * (255.0 / 1000.0);
    float pot4Value = pot4Voltage * (255.0 / 1000.0);
    
    //Converts decimal value from conversion to be a integer between 0-255
    potValues_8bit[0] = pot1Value;
    potValues_8bit[1] = pot2Value;
    potValues_8bit[2] = pot3Value;
    potValues_8bit[3] = pot4Value;
    
}

// Function to read all potentiometers and convert value to 8-bit number after averaging for smoothing
void readPotentiometersAverage() {
    
    //Potentiometer Readings
    int pot1Voltage = analogRead(POT_1_PIN);
    int pot2Voltage = analogRead(POT_2_PIN);
    int pot3Voltage = analogRead(POT_3_PIN);
    int pot4Voltage = analogRead(POT_4_PIN);
    
    //Convert analog reading (range of 0-1000) to a 8-bit value (0.0-255.0), it will be a decimal
    int pot1Value = pot1Voltage * (255.0 / 1000.0);
    int pot2Value = pot2Voltage * (255.0 / 1000.0);
    int pot3Value = pot3Voltage * (255.0 / 1000.0);
    int pot4Value = pot4Voltage * (255.0 / 1000.0);
    Serial.print(pot1Value);
    Serial.print(",");

    //subtract the last reading
    pot1Total = pot1Total - pot1Readings[readIndex];
    pot2Total = pot2Total - pot2Readings[readIndex];
    pot3Total = pot3Total - pot3Readings[readIndex];
    pot4Total = pot4Total - pot4Readings[readIndex];
    
    //add current value
    pot1Readings[readIndex] = pot1Value;
    pot2Readings[readIndex] = pot2Value;
    pot3Readings[readIndex] = pot3Value;
    pot4Readings[readIndex] = pot4Value;
    
    //add the reading to total
    pot1Total = pot1Total + pot1Readings[readIndex];
    pot2Total = pot2Total + pot2Readings[readIndex];
    pot3Total = pot3Total + pot3Readings[readIndex];
    pot4Total = pot4Total + pot4Readings[readIndex];
    
    //advance to next position in array
    readIndex = readIndex + 1;

    //if at end of array, wrap around
    if (readIndex >= numReadings) {
      readIndex = 0;
    }

    pot1Average = pot1Total/numReadings;
    pot2Average = pot2Total/numReadings;
    pot3Average = pot3Total/numReadings;
    pot4Average = pot4Total/numReadings;
    Serial.print(pot1Average);
    Serial.print(",");

    //Converts decimal value from conversion to be a integer between 0-255
    potValues_8bit[0] = pot1Average;
    Serial.println(potValues_8bit[0]);
    potValues_8bit[1] = pot2Average;
    potValues_8bit[2] = pot3Average;
    potValues_8bit[3] = pot4Average;
    
}

// Function to set mode of light
void setMode () {

  // Read mode button, HIGH or LOW
  int modeButtonState = digitalRead(BUTTON_1_PIN);

  // If button is pressed, subtract from counter
  if (modeButtonState == HIGH) {
    if (modeButtonCount > 0) {
      modeButtonCount--;
    }
    else {
      modeButtonCount = 2;
    }
    delay(buttonDelay);
  }
}


// Function to set pattern of light using pattern button (must be in pattern mode)
void setPattern () {

  // Read pattern button, HIGH or LOW
  int patternButtonState = digitalRead(BUTTON_2_PIN);
  
  // If button is pressed, subtract from counter
  if (patternButtonState == HIGH) {
    if (patternButtonCount > 0) {
      patternButtonCount--;
    }
    else {
      patternButtonCount = 4;
    }
    delay(buttonDelay);
  }

  // Set pattern based on button count
  switch (patternButtonCount) {
    case 0:
      juggle();
      break;
    case 1:
      bpm();
      break;
    case 2:
      sinelon();
      break;
    case 3:
      confetti();
      break;
    case 4:
      rainbow();
      break;
  }
}



// Function to read and track dimness button presses
void setDimness () {

  // Read dimness button HIGH or LOW
  int dimnessButtonState = digitalRead(BUTTON_3_PIN);

  // If button is pressed, subtract from counter
  if (dimnessButtonState == HIGH) {
    if (dimnessButtonCount > 0) {
      dimnessButtonCount--;
    }
    else {
      dimnessButtonCount = 10;
    }
    delay(buttonDelay);
  }

  // Set dimness value based on button count
  switch (dimnessButtonCount) {
    case 0:
      dimnessValue_8bit = 0;
      break;
    case 1:
      dimnessValue_8bit = 25;
      break;
    case 2:
      dimnessValue_8bit = 50;
      break;
    case 3:
      dimnessValue_8bit = 75;
      break;
    case 4:
      dimnessValue_8bit = 100;
      break;
    case 5:
      dimnessValue_8bit = 125;
      break;
    case 6:
      dimnessValue_8bit = 150;
      break;
    case 7:
      dimnessValue_8bit = 175;
      break;
    case 8:
      dimnessValue_8bit = 200;
      break;
    case 9:
      dimnessValue_8bit = 225;
      break;
    case 10:
      dimnessValue_8bit = 250;
      break;
  }
}


void setSaturation () {

  // Read saturation button HIGH or LOW
  int saturationButtonState = digitalRead(BUTTON_4_PIN);

  // If button is pressed, subtract from counter
  if (saturationButtonState == HIGH) {
    if (saturationButtonCount > 0) {
      saturationButtonCount--;
    }
    else {
      saturationButtonCount = 10;
    }
    delay(buttonDelay);
  }

  // Set saturation value based on button count
  switch (saturationButtonCount) {
    case 0:
      saturationValue_8bit = 0;
      break;
    case 1:
      saturationValue_8bit = 25;
      break;
    case 2:
      saturationValue_8bit = 50;
      break;
    case 3:
      saturationValue_8bit = 75;
      break;
    case 4:
      saturationValue_8bit = 100;
      break;
    case 5:
      saturationValue_8bit = 125;
      break;
    case 6:
      saturationValue_8bit = 150;
      break;
    case 7:
      saturationValue_8bit = 175;
      break;
    case 8:
      saturationValue_8bit = 200;
      break;
    case 9:
      saturationValue_8bit = 225;
      break;
    case 10:
      saturationValue_8bit = 250;
      break;
  }
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow(leds1, NUM_LEDS, gHue, 7);
  fill_rainbow(leds2, NUM_LEDS, gHue, 7);
  fill_rainbow(leds3, NUM_LEDS, gHue, 7);
  fill_rainbow(leds4, NUM_LEDS, gHue, 7);
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy(leds1, NUM_LEDS, 10);
  fadeToBlackBy(leds2, NUM_LEDS, 10);
  fadeToBlackBy(leds3, NUM_LEDS, 10);
  fadeToBlackBy(leds4, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds1[pos] += CHSV(gHue + random8(64), 200, 255);
  leds2[pos] += CHSV(gHue + random8(64), 200, 255);
  leds3[pos] += CHSV(gHue + random8(64), 200, 255);
  leds4[pos] += CHSV(gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy(leds1, NUM_LEDS, 20);
  fadeToBlackBy(leds2, NUM_LEDS, 20);
  fadeToBlackBy(leds3, NUM_LEDS, 20);
  fadeToBlackBy(leds4, NUM_LEDS, 20);
  int pos = beatsin16(13, 0, NUM_LEDS-1 );
  leds1[pos] += CHSV(gHue, 255, 192);
  leds2[pos] += CHSV(gHue, 255, 192);
  leds3[pos] += CHSV(gHue, 255, 192);
  leds4[pos] += CHSV(gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 120;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);
  for(int i = 0; i < NUM_LEDS; i++) { //9948
    leds1[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
    leds2[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
    leds3[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
    leds4[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy(leds1, NUM_LEDS, 20);
  fadeToBlackBy(leds2, NUM_LEDS, 20);
  fadeToBlackBy(leds3, NUM_LEDS, 20);
  fadeToBlackBy(leds4, NUM_LEDS, 20);
  byte dothue = 0;
  for(int i = 0; i < 8; i++) {
    leds1[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    leds2[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    leds3[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    leds4[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}
