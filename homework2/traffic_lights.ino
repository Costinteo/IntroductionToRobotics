/* number of stages */
#define STAGE_NUM                   4
/* max pins on arduino, for a characteristic array */
#define MAX_PINS                   15
/* blink interval for the led */
#define BLINK_INTERVAL            100
/* buzz interval for the buzzer */
#define BUZZER_INTERVAL           200
/* buzzer volume */
#define BUZZER_VOLUME               5


/* macros for stage numbers, STAGE_1 is 0 */
enum STAGES {
  STAGE_1,  /* green light for cars                 */
  STAGE_2,  /* yellow light for cars                */
  STAGE_3,  /* green light for pedestrians          */
  STAGE_4   /* green flashing light for pedestrians */
};

/* stage times in milliseconds */
enum STAGE_TIMES {
  T_STAGE_1 = 10000,
  T_STAGE_2 = 3000,
  T_STAGE_3 = 10000,
  T_STAGE_4 = 3000
};

enum PIN_CODES {
  BUTTON_PIN               =  4,
  BUZZER_PIN               =  3,
  RED_LED_CAR_PIN          = 11,
  YELLOW_LED_CAR_PIN       = 10,
  GREEN_LED_CAR_PIN        =  9,
  RED_LED_PEDESTRIAN_PIN   = 13,
  GREEN_LED_PEDESTRIAN_PIN = 12
};

/* timers in an array to access them incrementally in a generic function */
const STAGE_TIMES stageTimeArr[] = {T_STAGE_1, T_STAGE_2, T_STAGE_3, T_STAGE_4};
const PIN_CODES ledPinCodesArr[] = {
  RED_LED_CAR_PIN,
  YELLOW_LED_CAR_PIN,
  GREEN_LED_CAR_PIN,
  RED_LED_PEDESTRIAN_PIN,
  GREEN_LED_PEDESTRIAN_PIN
};

/* each index corresponds to an actual pin, 
 * making accessing faster by using the led pin codes 
 */
bool ledStateArr[MAX_PINS];

/* stage is changed incrementally and circulary */
uint8_t currentStage = STAGE_1;

/* variables regarding the button */
bool buttonState = HIGH;
bool lastButtonState = buttonState;
bool buttonWasPressed = false;

/* timer variable used by functions to cleanly time intervals */
uint32_t timer = 0;

/* function simply checks if button press just happened */
bool isButtonBeingPressed() {
  return (lastButtonState == HIGH && buttonState == LOW);
}

/* time with millis() only if timer isn't already used */
void saveTimer() {
  if (!timer) timer = millis();
}

void resetTimer() {
  timer = 0;
}

uint16_t getElapsed() {
  return millis() - timer;
}

/* handle each stage time */
void handleStageTimeExpired() {
  if (buttonWasPressed && getElapsed() > stageTimeArr[currentStage]) {

    /* special case for last stage, because it's about to loop around */
    if (currentStage == STAGE_NUM - 1) {
      buttonWasPressed = false; /* reset button press for stage 4 */
    }

    /* increment stage and reset timer */
    resetTimer();
    incrementStage();
    
  }
}

/* simply blink a led pin according to interval */
void blinkLed(const PIN_CODES& ledToBlink, const uint32_t& blinkInterval) {
  static uint32_t lastBlinked;
  
  if (millis() - lastBlinked > blinkInterval) {
    lastBlinked = millis();
    ledStateArr[ledToBlink] ^= HIGH; /* XOR with 1, basically inverting the boolean state */
  }
}

/* increase stage AND loop around circulary using % */
void incrementStage() {
  ++currentStage;
  currentStage %= STAGE_NUM;
}

/* reset buzzer to no sound, just in case it gets stuck */
void resetBuzz(const PIN_CODES& buzzerPin) {
  analogWrite(buzzerPin, LOW);
}

/* buzz at certain msFrequency
 * this function is unnecessarily complicated,
 * because at first I wanted to write a simple tone() function
 * (I failed)
 */
void buzz(const PIN_CODES& buzzerPin, const uint16_t& msFrequency) {  
  static uint32_t highTimer = 0;
  static uint32_t lowTimer = 0;

  if (!highTimer && !lowTimer) {
    analogWrite(buzzerPin, BUZZER_VOLUME);
    highTimer = millis();
    
  }

  if (millis() - highTimer >= msFrequency && !lowTimer) {
    analogWrite(buzzerPin, LOW);
    lowTimer = millis();
  }

  if (millis() - lowTimer >= msFrequency && highTimer && lowTimer) {
    highTimer = lowTimer = 0; 
  }
}

void setup() {
  
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  
  for (const PIN_CODES& ledPinCode : ledPinCodesArr) {
    pinMode(ledPinCode, OUTPUT);
    ledStateArr[ledPinCode] = LOW; /* start all leds on LOW */
  }

  Serial.begin(9600);
  
}

void loop() {

  lastButtonState = buttonState;
  buttonState = digitalRead(BUTTON_PIN);

  /* continuously write the led states to the pins */
  for (const PIN_CODES& ledPinCode : ledPinCodesArr) {
    digitalWrite(ledPinCode, ledStateArr[ledPinCode]);
  }
  
  switch(currentStage) {
    case STAGE_1:

      /* remnants from stage 4 */
      resetBuzz(BUZZER_PIN);
      ledStateArr[GREEN_LED_PEDESTRIAN_PIN] =   LOW;
      ledStateArr[RED_LED_CAR_PIN]          =   LOW;

      /* leds for stage 1 */
      ledStateArr[GREEN_LED_CAR_PIN]        =  HIGH;
      ledStateArr[RED_LED_PEDESTRIAN_PIN]   =  HIGH;
      
      if (!buttonWasPressed && isButtonBeingPressed()) {
        buttonWasPressed = true;
        saveTimer();
      } 

      handleStageTimeExpired();
      
      break;
    case STAGE_2:

      /* leds for stage 2 */
      ledStateArr[GREEN_LED_CAR_PIN]        =   LOW;
      ledStateArr[YELLOW_LED_CAR_PIN]       =  HIGH;
      
      saveTimer();
      handleStageTimeExpired();
      
      break;
    case STAGE_3:

      /* leds for stage 3 */
      ledStateArr[YELLOW_LED_CAR_PIN]       =   LOW;
      ledStateArr[RED_LED_CAR_PIN]          =  HIGH;
      ledStateArr[RED_LED_PEDESTRIAN_PIN]   =   LOW;
      ledStateArr[GREEN_LED_PEDESTRIAN_PIN] =  HIGH;

      buzz(BUZZER_PIN, BUZZER_INTERVAL);

      saveTimer();
      handleStageTimeExpired();

      break;
    case STAGE_4:
    
      blinkLed(GREEN_LED_PEDESTRIAN_PIN, BLINK_INTERVAL);

      /* we use blink interval here to match the two intervals */
      /* it's safer because there is no risk of mismatching defines or using magic numbers */
      buzz(BUZZER_PIN, BLINK_INTERVAL);
      
      saveTimer();
      handleStageTimeExpired();

      break;
    default:
      Serial.println("UNKNOWN STAGE!");
      exit(1);
    
  }
}
