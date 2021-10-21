/* macros for usual values */
#define POT_PIN_INPUT_MAX 1023
#define LED_INTENSITY_MAX 255

/* macros for the pins */
enum PIN_CODE {
  POT_PIN_1 = A0,
  POT_PIN_2 = A1,
  POT_PIN_3 = A2,
  
  RED_PIN = 11,
  GREEN_PIN = 10,
  BLUE_PIN = 9
};

/* struct for colour values */
struct Colour {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
};

/* global variables */
Colour ledColourIntensity;

uint8_t currentIntensityPercent = 10;

/*
 * function that takes as param an input from a potentiometer in range [0,1024)
 * and returns it normalized to the range [0,256)
 * before returning, it also multiplies with intensityPercent, so you can restrict the range further
 * by default, intensityPercent is 100%
 * 
 */
uint8_t normalizeInputFromPot(uint16_t potInput, uint8_t intensityPercent = 100) {
  return potInput / (POT_PIN_INPUT_MAX / LED_INTENSITY_MAX) * (intensityPercent % 100 / 100.0);
}

#include "test.h"
/* setup function */
void setup() {
  Serial.begin(9600);
  Serial.println(func());
  /* input pins */
  pinMode(POT_PIN_1, INPUT);
  pinMode(POT_PIN_2, INPUT);
  pinMode(POT_PIN_3, INPUT);

  /* output pins */
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  
}


/* loop function */
void loop() {

  /* input from potentiometers */
  ledColourIntensity.red = normalizeInputFromPot(analogRead(POT_PIN_1), currentIntensityPercent);
  ledColourIntensity.green = normalizeInputFromPot(analogRead(POT_PIN_2), currentIntensityPercent);
  ledColourIntensity.blue = normalizeInputFromPot(analogRead(POT_PIN_3), currentIntensityPercent);

  /* output to RGB led */
  analogWrite(RED_PIN, ledColourIntensity.red);
  analogWrite(GREEN_PIN, ledColourIntensity.green);
  analogWrite(BLUE_PIN, ledColourIntensity.blue);
  
}
