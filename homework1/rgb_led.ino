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
float intensityMultiplier = 0.1;    /* LED INTENSITY MULTIPLIER, USE TO LOWER INTENSITY FURTHER */
uint16_t redPotInput;
uint16_t greenPotInput;
uint16_t bluePotInput;

/* setup function */
void setup() {
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
  redPotInput = analogRead(POT_PIN_1);
  greenPotInput = analogRead(POT_PIN_2);
  bluePotInput = analogRead(POT_PIN_3);
  
  /* we normalize the input value using the map() function */
  /* we also multiply it by intensityMultiplier (which is a float value between 0 and 1) */
  /* this is done so the LED doesn't burn my eyes all the time */
  ledColourIntensity.red = intensityMultiplier * map(redPotInput, 0, POT_PIN_INPUT_MAX, 0, LED_INTENSITY_MAX);
  ledColourIntensity.green = intensityMultiplier * map(greenPotInput, 0, POT_PIN_INPUT_MAX, 0, LED_INTENSITY_MAX);
  ledColourIntensity.blue = intensityMultiplier * map(bluePotInput, 0, POT_PIN_INPUT_MAX, 0, LED_INTENSITY_MAX);

  /* output to RGB led */
  analogWrite(RED_PIN, ledColourIntensity.red);
  analogWrite(GREEN_PIN, ledColourIntensity.green);
  analogWrite(BLUE_PIN, ledColourIntensity.blue);
  
}
