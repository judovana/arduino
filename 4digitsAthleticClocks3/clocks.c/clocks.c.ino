#define outpin 13  // audio out to speaker or amp
//17 for C0 - 7902 for h8
//261 for c4- 493 for h4
//https://muted.io/note-frequencies/
void freqout(int freq, int t)  // freq in hz, t in ms
{
  Serial.print("playing ");
  Serial.print(freq);
  Serial.print(" Hz for ");
  Serial.print(t);
  Serial.println(" ms");
  int hperiod;  //calculate 1/2 period in us
  long cycles, i;
  pinMode(outpin, OUTPUT);                 // turn on output pin
  hperiod = (500000 / freq) - 7;           // subtract 7 us to make up for digitalWrite overhead
  cycles = ((long)freq * (long)t) / 1000;  // calculate cycles
  for (i = 0; i <= cycles; i++) {          // play note for t ms
    digitalWrite(outpin, HIGH);
    delayMicroseconds(hperiod);
    digitalWrite(outpin, LOW);
    delayMicroseconds(hperiod - 1);  // - 1 to make up for digitaWrite overhead
  }
  pinMode(outpin, INPUT);  // shut off pin to avoid noise from other operations
}

#include <Keypad.h>
const byte rows = 4;
const byte cols = 4;
char keys[rows][cols] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
byte rowPins[rows] = { 12, 11, 10, 9 };
byte colPins[cols] = { 8, 4, 3, 2 };
Keypad keypad_1 = Keypad(makeKeymap(keys), rowPins, colPins, rows, cols);

#include <Adafruit_NeoPixel.h>
#define PIN_NUMBERS 5
#define PIN_DELIMITER 7
#define LED_COUNT_PER_SRIP 60
#define LED_COUNT LED_COUNT_PER_SRIP * 4
#define LED_COUNT_DEL 9
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, PIN_NUMBERS, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel del = Adafruit_NeoPixel(LED_COUNT_DEL, PIN_DELIMITER, NEO_GRB + NEO_KHZ800);

int brightness = 27 * 2;
int mode = 1;
int setTime = 3600;
int runningTime = 0;  //seconds
                      //runningTime= 5200; //test
int setupState = 0;
int setupTimeOutMax = 100;

struct ParsedTime {
  int md1;
  int md2;  //minute digit 1, minute digit2
  int sd1;  //seconds digit 1, seconds digit 2
  int sd2;
};

void setup() {
  Serial.begin(9600);
  del.begin();
  del.show();
  strip.begin();
  strip.show();
  while (1) {
    char key = keypad_1.getKey();
    int a = (int)key;
    if (a >= 35) {
      Serial.print("Pressed: ");
      Serial.println(key);
      if (key == '*') {
        setupState = 1;
      }
    }
    if (setupState == 0) {
      runtimeMode();
    } else {
      setupMode();
    }
  }
}

void setupMode() {
  freqout(493, 50);
  int setupTimeOut = 0;
  int pageSetupSelect = 0;
  while (1) {
    char key = keypad_1.getKey();
    int a = (int)key;
    ////pritning, only twice per second, otherwise it will go mad
    if (setupTimeOut % 5 == 0) {
      clearLEDs();
      if (setupState == 1) {  //time seting
        ParsedTime current = parseTime(setTime);
        showNumber(current.md1, 0, pageSetupSelect == 0 ? brightness : 0, pageSetupSelect != 0 ? brightness : 0, 0);
        showNumber(current.md2, 1, pageSetupSelect == 1 ? brightness : 0, pageSetupSelect != 1 ? brightness : 0, 0);
        showNumber(current.sd1, 2, pageSetupSelect == 2 ? brightness : 0, pageSetupSelect != 2 ? brightness : 0, 0);
        showNumber(current.sd2, 3, pageSetupSelect == 3 ? brightness : 0, pageSetupSelect != 3 ? brightness : 0, 0);
      }
      if (setupState == 2) {  //time seting
        showNumber(mode, 0, pageSetupSelect == 0 ? brightness : 0, pageSetupSelect != 0 ? brightness : 0, 0);
        showNumber(brightness / 27, 1, pageSetupSelect == 1 ? brightness : 0, pageSetupSelect != 1 ? brightness : 0, 0);
      }
      strip.show();
    }
    if (a >= 35) {
      Serial.print("Pressed: ");
      Serial.println(key);
      setupTimeOut = -1;
      if (key == '#') {
        setupState = 0;
        break;
      }
      if (key == '*') {
        setupState += 1;
        if (setupState > 2) {
          setupState = 1;
        }
      }
      //reacting
      if (setupState == 1) {  //time seting
        ParsedTime current = parseTime(setTime);
        if (a >= 48 || a <= 57) { /*0-9*/
          int pressedNumberToAdjust = a - 48;
          if (pageSetupSelect == 0) {
            current.md1 = pressedNumberToAdjust;
          }
          if (pageSetupSelect == 1) {
            current.md2 = pressedNumberToAdjust;
          }
          if (pageSetupSelect == 2) {
            current.sd1 = pressedNumberToAdjust;
          }
          if (pageSetupSelect == 3) {
            current.sd2 = pressedNumberToAdjust;
          }
          setTime = calcTime(current);
          pageSetupSelect++;
          if (pageSetupSelect > 3) {
            pageSetupSelect = 0;
          }
          resetMode();
        }
      }
      if (setupState == 2) {  //time seting
        ParsedTime current = parseTime(setTime);
        if (a >= 48 || a <= 57) { /*0-9*/
          int pressedNumberToAdjust = a - 48;
          if (pageSetupSelect == 0) {
            if (pressedNumberToAdjust % 2 == 1) {
              mode = 1;
            } else {
              mode = -1;
            }
            resetMode();
          }
          if (pageSetupSelect == 1) {
            brightness = pressedNumberToAdjust * 27;
          }
          pageSetupSelect++;
          if (pageSetupSelect > 1) {
            pageSetupSelect = 0;
          }
        }
      }
    }
    setupTimeOut++;
    Serial.println(setupTimeOut);
    if (setupTimeOut > setupTimeOutMax) {
      setupState = 0;
      break;
    }
    delay(99);
  }
}

int calcTime(struct ParsedTime parsed) {
  return parsed.md1 * 10 * 60 + parsed.md2 * 60 + parsed.sd1 * 10 + parsed.sd2;
}

struct ParsedTime parseTime(int seconds) {
  int second = seconds % 60;
  int minute = seconds / 60;
  int sd1 = second / 10;
  int sd2 = second % 10;
  int md1 = minute / 10;
  int md2 = minute % 10;
  struct ParsedTime parsed = { md1, md2, sd1, sd2 };
  return parsed;
}

void runtimeMode() {
  ParsedTime parsed = parseTime(runningTime);
  debugTime(parsed);
  timeMode(parsed);
  runningTime += mode;
  //runningTime %= 120; //test, Reset x after 2minutes
  //runningTime %= 5400; //Reset x after 90minutes
  runningTime %= setTime;  //Reset x after 90minutes
  if (runningTime < 0) {
    runningTime = setTime;
  }
  if (runningTime == 0) { /*FIXME this is not viable, is causing dealy in counter*/
    freqout(4000, 100);   /*It is here for hackish non zero only*/
  }
  Serial.print(runningTime);
  Serial.print("/");
  Serial.println(setTime);
}

void timeMode(ParsedTime parsed) {
  for (int i = 0; i < 10; i++) {
    deldel(i);
    del.show();
    delay(99);
  }
  clearLEDs();
  showNumberWithDeadline(parsed.md1, 0, 5);
  showNumberWithDeadline(parsed.md2, 1, 9);
  showNumberWithDeadline(parsed.sd1, 2, 5);
  showNumberWithDeadline(parsed.sd2, 3, 9);
  strip.show();
  delay(9);  //10*99+9 a bit faster is better then a bit slower
}

void showNumberWithDeadline(int value, int display, int max) {
  int r = 0;
  int g = 0;
  int b = 0;
  if (max <= 0) {
    b = brightness;
  } else if (max == 1) {
    if (value == 0) {
      g = brightness;
    } else {
      r = brightness;
    }
  } else {
    int delimiter = (max + 1) / 2;
    //0     1  2   (2+1)/2=1
    //01    2  3   (3+1)/2=2
    //01    2  34  (4+1)/2=2
    //012   3  45  (5+1)/2=3
    //int sixDelimiter = 3;
    //012   3  56
    //0123  4  567
    //0123  4  5678
    //01234 5  6789
    //int tenDelimiter = 5;
    if (value < delimiter) {  //in 60 minutes, thsi behaves like  sd1 in 90 minutes, it would be like md1 and sd1
      g = brightness;
    } else if (value > delimiter) {
      r = brightness;
    } else {
      b = brightness;
    }
  }
  if (mode < 0) {
    //swap in countdown mode
    int swapRG = g;
    g = r;
    r = swapRG;
  }
  showNumber(value, display, r, g, b);
}

void showNumberRed(int value, int display) {
  showNumber(value, display, brightness, 0, 0);
}

void showNumberGreen(int value, int display) {
  showNumber(value, display, 0, brightness, 0);
}

void showNumberBlue(int value, int display) {
  showNumber(value, display, 0, 0, brightness);
}

void showNumber(int value, int display, int r, int g, int b) {
  debugNumberShow(value, display, r, g, b);
  int number = abs(value);  //Remove negative signs and any decimals
  if (value == -1) {
    number = -1;
  }
  if (number == 0 || number == 2 || number == 6 || number == 8) {
    segmentN(display, 0, r, g, b);
  }
  if (number == 0 || number == 2 || number == 3 || number == 5 || number == 6 || number == 8) {
    segmentN(display, 1, r, g, b);
  }
  if (number == 0 || number == 1 || number == 3 || number == 4 || number == 5 || number == 6 || number == 7 || number == 8 || number == 9 || number == -1) {
    segmentN(display, 2, r, g, b);
  }
  if (number == 0 || number == 1 || number == 2 || number == 3 || number == 4 || number == 7 || number == 8 || number == 9 || number == -1) {
    segmentN(display, 3, r, g, b);
  }
  if (number == 0 || number == 2 || number == 3 || number == 5 || number == 7 || number == 8 || number == 9) {
    segmentN(display, 4, r, g, b);
  }
  if (number == 0 || number == 4 || number == 5 || number == 6 || number == 8 || number == 9) {
    segmentN(display, 5, r, g, b);
  }
  if (number == 2 || number == 3 || number == 4 || number == 5 || number == 6 || number == 8 || number == 9 || number == -1) {
    segmentN(display, 6, r, g, b);
  }
}

//id = 0,1,2,4     n=0,1,2,3,4,5,6,7
void segmentN(int id, int n, int r, int g, int b) {
  int init = id * LED_COUNT_PER_SRIP;
  int fix = 0;
  if (n > 2) {
    fix++;
  }
  if (n > 5) {
    fix++;
  }
  showSequence(init + fix + (n * 8), init + fix + (n * 8) + 7, r, g, b);
}

void showSequence(int from, int to, int r, int g, int b) {
  for (int i = from; i <= to; i++) {
    strip.setPixelColor(i, r, g, b);
  }
}

void deldel(int xtime) {
  for (int i = 0; i < LED_COUNT_DEL; i++) {
    del.setPixelColor(i, 0);
  }
  int x = xtime % 10;
  if (x == 9) {
    for (int i = 1; i < LED_COUNT_DEL - 1; i++) {
      del.setPixelColor(i, brightness / 2, brightness / 2, brightness / 2);
    }
  } else {
    int color = x / 3;
    int r = 0;
    int g = 0;
    int b = 0;
    if (color == 0) {
      r = brightness;
      g = 0;
      b = 0;
    } else if (color == 1) {
      r = 0;
      g = 0;
      b = brightness;
    } else if (color == 2) {
      r = 0;
      g = brightness;
      b = 0;
    }
    if (x % 3 == 0) {
      del.setPixelColor(0, r, g, b);
      del.setPixelColor(8, r, g, b);
    } else if (x % 3 == 1) {
      del.setPixelColor(0, r, g, b);
      del.setPixelColor(1, r, g, b);
      del.setPixelColor(2, r, g, b);
      del.setPixelColor(6, r, g, b);
      del.setPixelColor(7, r, g, b);
      del.setPixelColor(8, r, g, b);
    } else if (x % 3 == 2) {
      for (int i = 0; i < LED_COUNT_DEL; i++) {
        del.setPixelColor(i, r, g, b);
      }
    }
  }
}


void clearLEDs() {
  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, 0);
  }
}

void resetMode() {
  if (mode > 0) {
    runningTime = 0;
  }
  if (mode < 0) {
    runningTime = setTime;
  }
}

void debugTime(ParsedTime parsed) {
  Serial.print("time: ");
  Serial.print(parsed.md1);
  Serial.print(parsed.md2);
  Serial.print(":");
  Serial.print(parsed.sd1);
  Serial.println(parsed.sd2);
}

void debugNumberShow(int value, int display, int r, int g, int b) {
  Serial.print("Showing: ");
  Serial.print(value);
  Serial.print(" at ");
  Serial.print(display);
  Serial.print(" in ");
  Serial.print(r);
  Serial.print(",");
  Serial.print(g);
  Serial.print(",");
  Serial.print(b);
  Serial.println("");
}