/*
  QuadLight Controller

  Controller for four RGB LED strips, color, pattern, brightness, etc.
  Board: Adafruit ItsyBitsy 5V
  Programmer: AVRISP mkII

  Current Build Status
  OK - Built in LED status on ItsyBitsy 5V
  OK - LED Array functionality using NeoPixelBus, N=30 Neopixel Strand

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
#define BRIGHTNESS 100;

//Define the array of leds to set/clear LED data
CRGB leds1[NUM_LEDS];
CRGB leds2[NUM_LEDS];
CRGB leds3[NUM_LEDS];
CRGB leds4[NUM_LEDS];

#define BUTTON_1_PIN 12 //Strand Mode button, individual control or universal control
#define BUTTON_2_PIN 11 //Pattern Select button
#define BUTTON_3_PIN 6 //Dimmer
#define BUTTON_4_PIN 4 //Saturation

#define POT_1_PIN A0 // Knob 1 pin
#define POT_2_PIN A1 // Knob 2 pin
#define POT_3_PIN A2 // Knob 3 pin
#define POT_4_PIN A3 // Knob 4 pin

// Initialize poteniometer color values from each pot as an array
uint8_t potValues_8bit[] = {0, 0, 0, 0};
// Initialize dimness and saturation values
uint8_t dimnessValue_8bit = 255;
uint8_t saturationValue_8bit = 255;

// Initialize the dimmer and saturation variables, initialize to full brightness and saturation
boolean strandModeAll = true; // Determines LED mode, master or individual control
int dimnessButtonCount = 10; // Determines brightness of each strand, 0-10, initialize to full brightness
int saturationButtonCount = 10; // Determines saturation of each strand, 0-10, initialize to full saturation

int buttonDelay = 200; // Delay for button presses to eliminate multiple registers




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
  
  // Read each button
  int modeButtonState = digitalRead(BUTTON_1_PIN);
  //int patternButtonState = digitalRead(BUTTON_2_PIN);

  // Read mode button, switch if button is pressed
  if (modeButtonState == HIGH) {
    strandModeAll = !strandModeAll;
    delay(buttonDelay);
  }
  
  // Check which mode LEDs are in
  // ALL CONTROL MODE
  if (strandModeAll) {

    setDimness();
    setSaturation();
    readPotentiometers();

    //Set all strands to color value of first potentiometer
    leds1On(potValues_8bit[0], saturationValue_8bit, dimnessValue_8bit);
    leds2On(potValues_8bit[0], saturationValue_8bit, dimnessValue_8bit);
    leds3On(potValues_8bit[0], saturationValue_8bit, dimnessValue_8bit);
    leds4On(potValues_8bit[0], saturationValue_8bit, dimnessValue_8bit);
  }
  // INDIVIDUAL MODE
  else {

    setDimness();
    setSaturation();
    readPotentiometers();
  
    leds1On(potValues_8bit[0], saturationValue_8bit, dimnessValue_8bit);
    leds2On(potValues_8bit[1], saturationValue_8bit, dimnessValue_8bit);
    leds3On(potValues_8bit[2], saturationValue_8bit, dimnessValue_8bit);
    leds4On(potValues_8bit[3], saturationValue_8bit, dimnessValue_8bit);
    
    }

}






/*___________ADDITIONAL FUNCTIONS_____________*/

// Functions to set colors for each strand
void leds1On(int h, int s, int v) {
  for(int i=0; i<NUM_LEDS; i++) {
    leds1[i] = CHSV(h, s, v);
  }
  FastLED.show();
}

void leds2On(int h, int s, int v) {
  for(int i=0; i<NUM_LEDS; i++) {
    leds2[i] = CHSV(h, s, v);
  }
  FastLED.show();
}

void leds3On(int h, int s, int v) {
  for(int i=0; i<NUM_LEDS; i++) {
    leds3[i] = CHSV(h, s, v);
  }
  FastLED.show();
}

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
    
    //Convert analog reading (range of 0-1000) to a 16-bit value (0.0-255.0), it will be a decimal
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
