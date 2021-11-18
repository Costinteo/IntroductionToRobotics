#include <stdarg.h>
#include <EEPROM.h>

#define BAUD_RATE                9600
/* Joystick upper treshold */
#define JOY_IN_UPPER_TRESHOLD     750
/* Joystick lower treshold */
#define JOY_IN_LOWER_TRESHOLD     350
/* number of displays */
#define DISPLAY_COUNT               4
/* number of digits in "alphabet" */
#define DIGIT_COUNT                16
/* debounce delay interval */
#define DEBOUNCE_DELAY            230
/* time given to current to drain from display */
#define DRAIN_TIME                  4
/* blink frequency interval */
#define BLINK_INTERVAL            300
/* save configuration at this interval */
#define SAVE_INTERVAL            1000

/* mapping digits to indices to use on digitArray */
enum EXTRA_DIGITS {
  A = 10,
  B,
  C,
  D,
  E,
  F,
  DP
};

enum SHIFT_PIN  {
  DATA_PIN  = 12, /* DS   */
  LATCH_PIN = 11, /* STCP */
  CLOCK_PIN = 10, /* SHCP */
};

enum DISPLAY_PIN {
  SEG_D1    =  7,
  SEG_D2    =  6,
  SEG_D3    =  5,
  SEG_D4    =  4
};

enum JOY_PIN {
  JOY_X     = A4,
  JOY_Y     = A5,
  JOY_SW    =  2
};

/* digit array containing all bitstring representations */
uint8_t digitArray[DIGIT_COUNT] = {
// A B C D E F G DP 
   B11111100, // 0
   B01100000, // 1
   B11011010, // 2
   B11110010, // 3
   B01100110, // 4
   B10110110, // 5
   B10111110, // 6
   B11100000, // 7
   B11111110, // 8
   B11110110, // 9
   B11101110, // A
   B00111110, // B
   B10011100, // C
   B01111010, // D
   B10011110, // E
   B10001110, // F
};


/* function that prints variable number of arguments (like printf)
 * I tried making it nicer. I failed miserably. I kinda hate it but I need it for debug.
 */
void printArgs(const char * argTypes, ...) {
  va_list vl;
  va_start(vl, argTypes);

  for (int i = 0; argTypes[i] != '\0'; ++i) {
    union Printable_t {
         int     i;
         float   f;
         char    c;
         char   *s;
      } Printable;

      switch( argTypes[i] ) { 
         case 'i':
             Printable.i = va_arg(vl, int);
             Serial.print(Printable.i);
         break;

         case 'f':
             Printable.f = va_arg(vl, double);
             Serial.print(Printable.f);
         break;

         case 'c':
             Printable.c = va_arg(vl, int);
             Serial.print(Printable.c);
         break;

         case 's':
             Printable.s = va_arg(vl, char *);
             Serial.print(Printable.s);
         break;

         default:
             Serial.print(argTypes[i]);
         break;
      }
  }
  va_end(vl);
}

/* display stuff */
namespace Display {

  /* enum for selection mode */
  enum SELECTION_MODE {
    DISPLAY_SELECT =  LOW,
    DIGIT_SELECT   = HIGH
  };
  
  const DISPLAY_PIN displaySegments[] = { SEG_D1, SEG_D2, SEG_D3, SEG_D4 };

  SELECTION_MODE    selectionMode               =  DISPLAY_SELECT;
  uint8_t           currentDisplay              =               0;
  bool              currentDisplayState         =             LOW;
  /* currentDigit array stores the digits for each display */
  uint8_t           currentDigit[DISPLAY_COUNT] =              {};
  uint32_t          lastBlinked                 =               0;

  uint32_t          lastSaved                   =               0;

  /* light up specific display */
  void lightDisplay(uint8_t displayNumber) {
    for (const DISPLAY_PIN& pin : displaySegments) {
      digitalWrite(pin, HIGH);
    }
    bool state = LOW;
    /* use the special currentDisplayState if we're on currentDisplay, so it blinks */
    if (currentDisplay == displayNumber)
      state = currentDisplayState && selectionMode == DIGIT_SELECT;

    digitalWrite(displaySegments[displayNumber], state);
  }

  /* write to register */
  void writeReg(uint8_t digit) {
    digitalWrite(LATCH_PIN, LOW);
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, digit);
    digitalWrite(LATCH_PIN, HIGH);
  }

  /* get the passed display's digit bitstring representation */
  uint8_t getDigitBinary(uint8_t disp) { 
    return digitArray[currentDigit[disp]]; 
    
  }

  /* write the digits to the displays */
  void writeDigits() {
    for (uint8_t disp = 0; disp < DISPLAY_COUNT; ++disp) {
      uint8_t dToWrite = getDigitBinary(disp);
      /* if we're on currentDisplay, add the period */
      if (currentDisplay == disp) {
        dToWrite |= 1 ;
        /* also blink it if we're in selectionMode "DISPLAY_SELECT" */
        dToWrite ^= (currentDisplayState == LOW && selectionMode == DISPLAY_SELECT);
      }
      writeReg(dToWrite);
      lightDisplay(disp);
      /* used to let current drain */
      delay(DRAIN_TIME);
    }
  }

  /* flips the currentDisplay state on an interval basis */
  void flipSelected() {
    if (millis() - lastBlinked > BLINK_INTERVAL) {
      currentDisplayState ^= HIGH;
      lastBlinked = millis();
    }
  }

  void saveDisplayState() {

    if (millis() - lastSaved < SAVE_INTERVAL) return;
    lastSaved = millis();
    
    /* we're gonna use addr to iterate through display indices and also write to on arduino */
    /* a bit confusing, honestly, but it works. I'm not gonna double variable this loop just for this */
    for (uint8_t addr = 0; addr < DISPLAY_COUNT; ++addr) {
      EEPROM.write(addr, currentDigit[addr]);
    }
  }

  void loadDisplayState() {
    /* see above */
    for (uint8_t addr = 0; addr < DISPLAY_COUNT; ++addr) {
      currentDigit[addr] = EEPROM.read(addr);
    }
  }
  
}; /* end of Display namespace */

/* joystick stuff */
namespace Joystick {

  /* have to use this namespace here */
  /* or all the lines become twice as long */
  using namespace Display;
  
  uint16_t          joyInputX;
  uint16_t          joyInputY;
  bool              joyInputSW;
  bool              lastInputSW;
  uint32_t          lastSwitchTimer = 0;
  volatile bool     joyMoved        = false;

  /* switches selection mode */
  void switchMode() { selectionMode ^= 1; }
  /* decrements / increments the currently selected display's index */
  void decDisplay() { currentDisplay = currentDisplay ? --currentDisplay : DISPLAY_COUNT - 1; }
  void incDisplay() { currentDisplay = ++currentDisplay % DISPLAY_COUNT; }
  /* decrements / increments the currently selected display's digit */
  void decDigit()   { currentDigit[currentDisplay] = currentDigit[currentDisplay] ? --currentDigit[currentDisplay] : DIGIT_COUNT - 1; }
  void incDigit()   { currentDigit[currentDisplay] = ++currentDigit[currentDisplay] % DIGIT_COUNT; }

  /*
   * There's some crazy magic going on above! Basically the whole logic is meant to loop around.
   * If we increment out of bounds, it will MOD back in. 
   * If we decrement out of bounds, it will jump the currentDisplay / currentDigit to the highest value "displayIndex" / "digit". (based on DISPLAY_COUNT / DIGIT_COUNT)
   * Logic for it is a little bit inversed, because the displays are indexed from right to left.
   */

  void resetDebounceTimer() { lastSwitchTimer = millis(); }
  /* checks if debounceTimerExpired */
  bool debounceTimerExpired() { return (millis() - lastSwitchTimer > DEBOUNCE_DELAY); }

  /* resets the boolean joyMoved if it's in between the tresholds */
  void resetJoyMoved(uint16_t joyInput) {
    joyMoved = (joyInput < JOY_IN_LOWER_TRESHOLD || joyInput > JOY_IN_UPPER_TRESHOLD);
  }

  /* routine function for button interrupt */
  void handleButtonPress() {
    if (debounceTimerExpired()) {
      switchMode();
      resetDebounceTimer();
    }
  }
  
  void handleDisplayPick(uint16_t joyInput) {
    if (joyMoved) return;
    if (joyInput > JOY_IN_UPPER_TRESHOLD)      decDisplay();
    else if (joyInput < JOY_IN_LOWER_TRESHOLD) incDisplay();
  }

  void handleDigitPick(uint16_t joyInput) {
    if (joyMoved) return;
    if (joyInput > JOY_IN_UPPER_TRESHOLD)      decDigit();
    else if (joyInput < JOY_IN_LOWER_TRESHOLD) incDigit();
  }

  void handleJoyInput() {
    
    lastInputSW =  joyInputSW;
    joyInputX   =   analogRead(JOY_X);
    joyInputY   =   analogRead(JOY_Y);
    joyInputSW  =  digitalRead(JOY_SW);

    /* call specific handle function based on selectionMode */
    if (selectionMode == DISPLAY_SELECT) {
        handleDisplayPick(joyInputX);
    } else handleDigitPick(joyInputY);

    /* we reset joy moved based on X or Y depending on selectionMode */
    resetJoyMoved(selectionMode == DISPLAY_SELECT ? joyInputX : joyInputY);
  
    printArgs("i i i i\n", joyInputX, joyInputY, joyInputSW, selectionMode);
    printArgs("i i i\n", currentDisplay, currentDigit[currentDisplay], selectionMode);
  }


}; /* end of Joystick namespace */


void setup ()
{
  pinMode(DATA_PIN,  OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);

  pinMode(JOY_X,  INPUT);
  pinMode(JOY_Y,  INPUT);
  pinMode(JOY_SW, INPUT_PULLUP);

  for (const DISPLAY_PIN& pin : Display::displaySegments) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
  }

  /* might read trash values when it's run the first time, but I don't really care */
  Display::loadDisplayState();

  /* interrupt! */
  attachInterrupt(digitalPinToInterrupt(JOY_SW), Joystick::handleButtonPress, FALLING);
  Serial.begin(BAUD_RATE);

}

void loop()
{ 
  /* ready, camera, action! */
  Display::writeDigits();
  Display::flipSelected();
  Joystick::handleJoyInput();

  Display::saveDisplayState();
}
