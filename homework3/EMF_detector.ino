#define BUZZER_VOLUME           3
#define HI_BUZZ_FREQ            1
#define BUZZ_FREQ_MULTIPLIER   10
#define AVG_IN_SAMPLE_SIZE    300

enum PIN_CODES {
  GREEN_LO = 2,
  GREEN_MI,
  GREEN_HI,
  YELLOW_LO,
  YELLOW_MI,
  YELLOW_HI,
  RED_LO,
  RED_MI,
  RED_HI,
  BUZZER_PIN = 11,
  ANT_INPUT_PIN = A5
};

const PIN_CODES ledArr[] = { GREEN_LO, GREEN_MI, GREEN_HI, YELLOW_LO, YELLOW_MI, YELLOW_HI, RED_LO, RED_MI, RED_HI };
const uint8_t ledArrSize = sizeof(ledArr) / sizeof(PIN_CODES);

/*
 * buzz function that I've used in the previous homework
 * basically buzzes on and off at msFrequency intervals
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

/*
 * function that takes a pin and a sample size and
 * averages the next "sample size" inputs from that pin
 */
uint32_t getAverageRead(const PIN_CODES& pin, const uint16_t sampleSize) {
  uint32_t sum = 0;
  for (int i = 0; i < sampleSize; i++) {
    sum += analogRead(pin);
  }
  return sum / sampleSize;
}

void setup() {
  for (const PIN_CODES& pin : ledArr) {
    pinMode(pin, OUTPUT);
  }
  pinMode(ANT_INPUT_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  Serial.begin(9600);
}


void loop() {
  static uint32_t antInput;      /* the actual antenna input, averaged             */
  static uint8_t  ledLevel;      /* basically stores how many leds should light up */
  static uint8_t  buzzFreq;      /* how often should the buzzer buzz in ms         */

  antInput = getAverageRead(ANT_INPUT_PIN, AVG_IN_SAMPLE_SIZE);
  
  ledLevel = constrain(antInput, 0, ledArrSize);

  Serial.print("\n\nAntenna input value: ");
  Serial.print(antInput);
  Serial.print("\nLed level: ");
  Serial.print(ledLevel);

  /* this lights up the leds based on what ledLevel we have previously calculated */
  for (const PIN_CODES& pin : ledArr) {
    bool state = pin < ledArr[ledLevel]; /* current led state is basically         */
    digitalWrite(pin, state);            /* equal to the result of this comparison */
  }

  buzzFreq  = HI_BUZZ_FREQ;            /* this ensures that at the highest led level, buzzer buzzes very often                             */
  buzzFreq += (ledArrSize - ledLevel); /* sums to the frequency the distance to the highest level, to sound more seldomly at lower levels  */
  buzzFreq *= BUZZ_FREQ_MULTIPLIER;    /* multiplies to further make a bigger difference in the sound frequency for the human ear          */
  buzzFreq *= (ledLevel != 0);         /* multiply with boolean that verifies if ledLevel is 0, to avoid costly if branching               */
  buzz(BUZZER_PIN, buzzFreq);
}
