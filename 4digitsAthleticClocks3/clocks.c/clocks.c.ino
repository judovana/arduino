int laudness = 0;
int L = 1;
int R = 1;

int finalSound = 1;
int halfSound = 1;
int thirdSound = 1;
int threeTwoOneSound = 1;

void freqout(int freq, int t) {
  freqoutImpl(L == 1, R == 1, freq * (laudness + 1), t);
}

#define outpin1 13  // audio out to speaker or amp
#define outpin2 6   // stereo

//17 for C0 - 7902 for h8
//261 for c4- 493 for h4
//https://muted.io/note-frequencies/
void freqoutImpl(bool L, bool R, int freq, int t)  // freq in hz, t in ms
{
  if (L) {
    Serial.print("L");
  } else {
    Serial.print("x");
  }
  if (R) {
    Serial.print("R");
  } else {
    Serial.print("x");
  }
  Serial.print(" playing ");
  Serial.print(freq);
  Serial.print(" Hz for ");
  Serial.print(t);
  Serial.println(" ms");
  if (L == R && L == false) {
    delay(t);
    return;
  }
  int hperiod;  //calculate 1/2 period in us
  long cycles, i;
  if (L) {
    pinMode(outpin1, OUTPUT);  // turn on output pin
  }
  if (R) {
    pinMode(outpin2, OUTPUT);
  }
  hperiod = (500000 / freq) - 7;           // subtract 7 us to make up for digitalWrite overhead
  cycles = ((long)freq * (long)t) / 1000;  // calculate cycles
  for (i = 0; i <= cycles; i++) {          // play note for t ms
    if (L) {
      digitalWrite(outpin1, HIGH);
    }
    if (R) {
      digitalWrite(outpin2, HIGH);
    }
    delayMicroseconds(hperiod);
    if (L) {
      digitalWrite(outpin1, LOW);
    }
    if (R) {
      digitalWrite(outpin2, LOW);
    }
    delayMicroseconds(hperiod - 1);  // - 1 to make up for digitaWrite overhead
  }
  if (L) {
    pinMode(outpin1, INPUT);  // shut off pin to avoid noise from other operations
  }
  if (R) {
    pinMode(outpin2, INPUT);
  }
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

int soundMap[LED_COUNT_DEL] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

int brightness = 12;
//if true, then brightnes is loaded from eeprom
int saveBrightness = 0;  //false
int mode = -1;
int iterations = 0;
int runningIteration = 0;
int maxTimes = 0;
int currentMaxTime = 0;
#define MAX_TIMES 10
int setTime[MAX_TIMES] = { 300, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int runningTime = 0;  //seconds
unsigned long tStart = 0;

#define MAX_SUBSETUPS 1000
int setupStateGlobal = 0;
int setupTimeOutMax = 100;

const int saveCokie1 = 2738;
const int saveCokie2 = 1236;

#include <EEPROM.h>
void saveInt(int address, int i) {
  EEPROM.write(address, i >> 8);
  EEPROM.write(address + 1, i & 0xFF);
}
void save() {
  saveInt(0, saveCokie1);
  saveInt(2, saveBrightness);
  saveInt(4, brightness);
  saveInt(6, mode);
  saveInt(8, laudness);
  saveInt(10, L);
  saveInt(12, R);
  saveInt(14, finalSound);
  saveInt(16, halfSound);
  saveInt(18, thirdSound);
  saveInt(20, threeTwoOneSound);
  saveInt(22, iterations);
  saveInt(24, maxTimes);
  saveInt(26, saveCokie2);
  for (int i = 0; i < MAX_TIMES; i++) {
    saveInt(28 + (i * 2), setTime[i]);
  }
  Serial.println("saved");
}
int loadInt(int address) {
  return (EEPROM.read(address) << 8) + EEPROM.read(address + 1);
}
void load() {
  int cookie1 = loadInt(0);
  int cookie2 = loadInt(26);
  if (cookie1 == saveCokie1 && cookie2 == saveCokie2) {
    saveBrightness = loadInt(2);
    if (saveBrightness > 0) {
      brightness = loadInt(4);
      Serial.print("loaded bightness to:");
    } else {
      Serial.println("reset bightness to:");
    }
    Serial.println(brightness);
    mode = loadInt(6);
    laudness = loadInt(8);
    L = loadInt(10);
    if (L != 0) {
      L = 1;
    }
    R = loadInt(12);
    if (R != 0) {
      R = 1;
    }
    finalSound = loadInt(14);
    if (finalSound != 0) {
      finalSound = 1;
    }
    halfSound = loadInt(16);
    if (halfSound != 0) {
      halfSound = 1;
    }
    thirdSound = loadInt(18);
    if (thirdSound != 0) {
      thirdSound = 1;
    }
    threeTwoOneSound = loadInt(20);
    if (threeTwoOneSound != 0) {
      threeTwoOneSound = 1;
    }
    iterations = loadInt(22);
    if (iterations > 9 || iterations < 0) {
      iterations = 0;
    }
    maxTimes = loadInt(24);
    if (maxTimes > 9 || maxTimes < 0) {
      maxTimes = 0;
    }
    for (int i = 0; i < MAX_TIMES; i++) {
      setTime[i] = loadInt(28 + (i * 2));
    }
    Serial.println("loaded values");
  } else {
    Serial.println("Not loading, invalid magic bytes");
  }
  resetTimes(false);
}

struct ParsedTime {
  int md1;
  int md2;  //minute digit 1, minute digit2
  int sd1;  //seconds digit 1, seconds digit 2
  int sd2;
};

void resetTimes(bool play) {
  Serial.println("Reset: ");
  if (play) {
    freqout(593, 50);
  }
  resetMode();
}

void pause() {
  Serial.println("Paused: ");
  freqout(393, 50);
  while (1) {
    delay(99);
    char key = keypad_1.getKey();
    int a = (int)key;
    if (a >= 35) {
      if (key == 'D') {
        resetTimes(true);
      } else {
        freqout(293, 50);
      }
      break;
    }
  }
}

void setup() {
  Serial.begin(9600);
  load();
  del.begin();
  clearDelDel();
  del.show();
  strip.begin();
  clearLEDs();
  showNumber(iterations, 0, brightness, brightness, brightness);
  showNumber(1 /*like I*/, 1, brightness, 0, 0);
  showNumber(maxTimes, 2, brightness, brightness, brightness);
  showNumber(-65 /*like -A*/, 3, brightness, 0, 0);
  strip.show();
  delay(2000);
  while (1) {
    tStart = millis();
    Serial.print("Start: ");
    Serial.println(tStart);
    if (setupStateGlobal == 0) {
      bool wasIterationReset = runtimeMode();
      if (iterations != 0 && wasIterationReset) {
        if (runningIteration == 0) {
          freqout(1000, 50);
          freqout(800, 50);
          freqout(600, 50);
          freqout(400, 50);
          freqout(1200, 100);
          pause();
          resetMode();
        }
      }
    } else {
      setupMode();
    }
    Serial.print("Stop: ");
    Serial.println(millis());
  }
}

void setupMode() {
  freqout(493, 50);
  int setupTimeOut = 0;
  int pageSetupSelect = 0;
  while (1) {
    int setupStateLocal = setupStateGlobal / MAX_SUBSETUPS;  //holding real page 1(000) 2(0005) ...
    int dynamicSubsetup = setupStateGlobal % MAX_SUBSETUPS;  //holding subpage  0         5 ....
    char key = keypad_1.getKey();
    int a = (int)key;
    ////pritning, only twice per second, otherwise it will go mad
    if (setupTimeOut % 5 == 0) {
      Serial.print("menu page: ");
      Serial.print(setupStateGlobal);
      Serial.print("(");
      Serial.print(setupStateLocal);
      Serial.print("/");
      Serial.print(dynamicSubsetup);
      Serial.print(")");
      Serial.print(" page select: ");
      Serial.print(pageSetupSelect);
      Serial.println("");
      deldel(setupStateLocal, 0);
      clearLEDs();
      if (setupStateLocal == 1) {  //time seting
        ParsedTime current = parseTime(setTime[0]);
        showNumber(current.md1, 0, pageSetupSelect == 0 ? brightness : 0, pageSetupSelect != 0 ? brightness : 0, 0);
        showNumber(current.md2, 1, pageSetupSelect == 1 ? brightness : 0, pageSetupSelect != 1 ? brightness : 0, 0);
        showNumber(current.sd1, 2, pageSetupSelect == 2 ? brightness : 0, pageSetupSelect != 2 ? brightness : 0, 0);
        showNumber(current.sd2, 3, pageSetupSelect == 3 ? brightness : 0, pageSetupSelect != 3 ? brightness : 0, 0);
      }
      if (setupStateLocal == 2) {
        if (dynamicSubsetup == 0) {  //iteratins, times, nic, stopwatch/countdown
          showNumber(iterations, 0, pageSetupSelect == 0 ? brightness : 0, pageSetupSelect != 0 ? brightness : 0, 0);
          showNumber(maxTimes, 1, pageSetupSelect == 1 ? brightness : 0, pageSetupSelect != 1 ? brightness : 0, 0);
          showNumber(mode, 3, pageSetupSelect == 2 ? brightness : 0, pageSetupSelect != 2 ? brightness : 0, 0);
        } else {
          /*Dynamically set maxTimes; those will;have deldel in MODE 1!*/
          deldel(dynamicSubsetup, 1);
          ParsedTime current = parseTime(setTime[dynamicSubsetup]);
          showNumber(current.md1, 0, pageSetupSelect == 0 ? brightness : 0, pageSetupSelect != 0 ? brightness : 0, 0);
          showNumber(current.md2, 1, pageSetupSelect == 1 ? brightness : 0, pageSetupSelect != 1 ? brightness : 0, 0);
          showNumber(current.sd1, 2, pageSetupSelect == 2 ? brightness : 0, pageSetupSelect != 2 ? brightness : 0, 0);
          showNumber(current.sd2, 3, pageSetupSelect == 3 ? brightness : 0, pageSetupSelect != 3 ? brightness : 0, 0);
          /*end dynamic maxTimes*/
        }
      }
      if (setupStateLocal == 3) {  //brightness
        ParsedTime parsedbr = parseInt(brightness);
        showNumber(saveBrightness, 0, pageSetupSelect == 0 ? brightness : 0, pageSetupSelect != 0 ? brightness : 0, 0);
        showNumber(parsedbr.md2, 1, pageSetupSelect == 1 ? brightness : 0, pageSetupSelect != 1 ? brightness : 0, 0);
        showNumber(parsedbr.sd1, 2, pageSetupSelect == 2 ? brightness : 0, pageSetupSelect != 2 ? brightness : 0, 0);
        showNumber(parsedbr.sd2, 3, pageSetupSelect == 3 ? brightness : 0, pageSetupSelect != 3 ? brightness : 0, 0);
      }
      if (setupStateLocal == 4) {  //pitch nothing L/R
        ParsedTime parsedbr = parseInt(brightness);
        showNumber(laudness, 0, pageSetupSelect == 0 ? brightness : 0, pageSetupSelect != 0 ? brightness : 0, 0);
        showNumber(L, 2, pageSetupSelect == 1 ? brightness : 0, pageSetupSelect != 1 ? brightness : 0, 0);
        showNumber(R, 3, pageSetupSelect == 2 ? brightness : 0, pageSetupSelect != 2 ? brightness : 0, 0);
      }
      if (setupStateLocal == 5) {  //sound types  final 1/2 2/3  3,2,1
        showNumber(finalSound, 0, pageSetupSelect == 0 ? brightness : 0, pageSetupSelect != 0 ? brightness : 0, 0);
        showNumber(halfSound, 1, pageSetupSelect == 1 ? brightness : 0, pageSetupSelect != 1 ? brightness : 0, 0);
        showNumber(thirdSound, 2, pageSetupSelect == 2 ? brightness : 0, pageSetupSelect != 2 ? brightness : 0, 0);
        showNumber(threeTwoOneSound, 3, pageSetupSelect == 3 ? brightness : 0, pageSetupSelect != 3 ? brightness : 0, 0);
      }
      strip.show();
      del.show();
    }
    if (a >= 35) {
      Serial.print("Pressed: ");
      Serial.print(key);
      Serial.print("/");
      Serial.println(a);
      setupTimeOut = -1;
      if (key == '#') {
        setupStateGlobal = 0;
        save();
        break;
      }
      if (key == '*') {
        pageSetupSelect = 0;
        if (setupStateLocal == 2) {
          setupStateGlobal++;
          if (setupStateGlobal - 2000 > maxTimes) {
            setupStateGlobal = 3000;
          }
        } else {
          setupStateGlobal += MAX_SUBSETUPS;
        }
        if (setupStateGlobal > 5 * MAX_SUBSETUPS) {
          setupStateGlobal = 1 * MAX_SUBSETUPS;
        }
        continue;
      }
      if (setupStateLocal == 2 && dynamicSubsetup != 0) {
        //a==65..D==68
        if (a > 64 && a < 69) {
          pageSetupSelect = a - 65;
        }
      } else {
        if (key == 'A') {
          pageSetupSelect = 0;
        }
        if (key == 'B' && (setupStateLocal == 1 || setupStateLocal == 2 || setupStateLocal == 3 || setupStateLocal == 5)) {
          pageSetupSelect = 1;
        }
        if (key == 'B' && (setupStateLocal == 4)) {
          //empty
        }
        if (key == 'C' && (setupStateLocal == 1 || setupStateLocal == 3 || setupStateLocal == 5)) {
          pageSetupSelect = 2;
        }
        if (key == 'C' && (setupStateLocal == 4)) {
          pageSetupSelect = 1;
        }
        if (key == 'C' && (setupStateLocal == 2)) {
          //empty
        }
        if (key == 'D' && (setupStateLocal == 1 || setupStateLocal == 3 || setupStateLocal == 5)) {
          pageSetupSelect = 3;
        }
        if (key == 'D' && (setupStateLocal == 2 || setupStateLocal == 4)) {
          pageSetupSelect = 2;
        }
      }
      //reacting
      if (setupStateLocal == 1) {  //time seting
        if (a >= 48 && a <= 57) {  /*0-9*/
          ParsedTime current = parseTime(setTime[0]);
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
          setTime[0] = calcTime(current);
          pageSetupSelect++;
          if (pageSetupSelect > 3) {
            pageSetupSelect = 0;
          }
          resetMode();
        }
      }
      if (setupStateLocal == 2 && dynamicSubsetup == 0) {  //iterations, maxTimes, nothing, stopwatch/coountdown mode
        if (a >= 48 && a <= 57) {                          /*0-9*/
          int pressedNumberToAdjust = a - 48;
          //iterations 0-9
          if (pageSetupSelect == 0) {
            iterations = pressedNumberToAdjust;
            resetMode();
          }
          //maxTimes 0-9
          if (pageSetupSelect == 1) {
            int oldMaxTimes = maxTimes;
            maxTimes = pressedNumberToAdjust;
            for (int x = oldMaxTimes; x <= maxTimes; x++) {
              /*if (setTime[x] == 0) */ {
                setTime[x] = setTime[oldMaxTimes];
              }
            }
            resetMode();
          }
          //stopwatch/coountdown mode
          if (pageSetupSelect == 2) {
            if (pressedNumberToAdjust % 2 == 1) {
              mode = 1;
            } else {
              mode = -1;
            }
            resetMode();
          }
          pageSetupSelect++;
          if (pageSetupSelect > 2) {
            pageSetupSelect = 0;
          }
        }
      }
      if (setupStateLocal == 2 && dynamicSubsetup != 0) {  //subtimes
        if (a >= 48 && a <= 57) {                          /*0-9*/
          ParsedTime current = parseTime(setTime[dynamicSubsetup]);
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
          setTime[dynamicSubsetup] = calcTime(current);
          pageSetupSelect++;
          if (pageSetupSelect > 3) {
            pageSetupSelect = 0;
          }
          resetMode();
        }
      }
      if (setupStateLocal == 3) {  //brightness
        if (a >= 48 && a <= 57) {  /*0-9*/
          ParsedTime parsedbr = parseInt(brightness);
          int pressedNumberToAdjust = a - 48;
          if (pageSetupSelect == 0) {
            if (pressedNumberToAdjust == 1) {
              saveBrightness = pressedNumberToAdjust;
            } else {
              saveBrightness = 0;
            }
          }
          if (pageSetupSelect == 1) {
            parsedbr.md2 = pressedNumberToAdjust;
            Serial.println(parsedbr.md2);
            if (parsedbr.md2 > 2) {
              parsedbr.md2 = 2;
            }
            Serial.println(parsedbr.md2);
          }
          if (pageSetupSelect == 2) {
            parsedbr.sd1 = pressedNumberToAdjust;
            if (parsedbr.md2 == 2) {
              if (parsedbr.sd1 > 4) {
                parsedbr.sd1 = 4;
              }
            }
          }
          if (pageSetupSelect == 3) {
            parsedbr.sd2 = pressedNumberToAdjust;
          }
          brightness = calcInt(parsedbr);
          if (brightness > 250) {
            brightness = 250;
          }
          pageSetupSelect++;
          if (pageSetupSelect > 3) {
            pageSetupSelect = 0;
          }
        }
      }
      if (setupStateLocal == 4) {  //pitch nothing L/R
        if (a >= 48 && a <= 57) {  /*0-9*/
          int pressedNumberToAdjust = a - 48;
          if (pageSetupSelect == 0) {
            laudness = pressedNumberToAdjust;
          }
          if (pageSetupSelect == 1) {
            if (pressedNumberToAdjust == 0) {
              L = pressedNumberToAdjust;
            } else {
              L = 1;
            }
          }
          if (pageSetupSelect == 2) {
            if (pressedNumberToAdjust == 0) {
              R = pressedNumberToAdjust;
            } else {
              R = 1;
            }
          }
          for (int xx = 0; xx < 3; xx++) {
            freqout(300, 50);
            delay(50);
            freqout(500, 50);
            delay(50);
            freqout(1000, 50);
            delay(50);
          }
          pageSetupSelect++;
          if (pageSetupSelect > 2) {
            pageSetupSelect = 0;
          }
        }
      }
      if (setupStateLocal == 5) {  //sound types  final 1/2 2/3  3,2,1
        if (a >= 48 && a <= 57) {  /*0-9*/
          int pressedNumberToAdjust = a - 48;
          if (pageSetupSelect == 0) {
            if (pressedNumberToAdjust == 0) {
              finalSound = pressedNumberToAdjust;
            } else {
              finalSound = 1;
              setSoundMap(0);
              setSoundMapPerTimeToFinal();
              for (int i = 0; i < 10; i++) {
                playOrWaitNthTenOfSecond(i);
              }
            }
          }
          if (pageSetupSelect == 1) {
            if (pressedNumberToAdjust == 0) {
              halfSound = pressedNumberToAdjust;
            } else {
              halfSound = 1;
              setSoundMap(0);
              setSoundMapPerTimeToOnehlaf();
              for (int i = 0; i < 10; i++) {
                playOrWaitNthTenOfSecond(i);
              }
            }
          }
          if (pageSetupSelect == 2) {
            if (pressedNumberToAdjust == 0) {
              thirdSound = pressedNumberToAdjust;
            } else {
              thirdSound = 1;
              setSoundMap(0);
              setSoundMapPerTimeToTwoThirds();
              for (int i = 0; i < 10; i++) {
                playOrWaitNthTenOfSecond(i);
              }
            }
          }
          if (pageSetupSelect == 3) {
            if (pressedNumberToAdjust == 0) {
              threeTwoOneSound = pressedNumberToAdjust;
            } else {
              threeTwoOneSound = 1;
              setSoundMap(0);
              setSoundMapPerTimeToThree();
              for (int i = 0; i < 10; i++) {
                playOrWaitNthTenOfSecond(i);
              }
              delay(80);
              setSoundMap(0);
              setSoundMapPerTimeToTwo();
              for (int i = 0; i < 10; i++) {
                playOrWaitNthTenOfSecond(i);
              }
              delay(80);
              setSoundMap(0);
              setSoundMapPerTimeToOne();
              for (int i = 0; i < 10; i++) {
                playOrWaitNthTenOfSecond(i);
              }
            }
          }
          pageSetupSelect++;
          if (pageSetupSelect > 3) {
            pageSetupSelect = 0;
          }
        }
      }
    }
    setupTimeOut++;
    Serial.println(setupTimeOut);
    if (setupTimeOut > setupTimeOutMax) {
      setupStateGlobal = 0;
      save();
      break;
    }
    delay(99);
  }
}

int calcTime(struct ParsedTime parsed) {
  int r = parsed.md1 * 10 * 60 + parsed.md2 * 60 + parsed.sd1 * 10 + parsed.sd2;
  debugCalc("Changed time ", parsed, r);
  return r;
}

struct ParsedTime parseTime(int seconds) {
  int second = seconds % 60;
  int minute = seconds / 60;
  int sd1 = second / 10;
  int sd2 = second % 10;
  int md1 = minute / 10;
  int md2 = minute % 10;
  struct ParsedTime parsed = { md1, md2, sd1, sd2 };
  debugParse("Changed time ", seconds, parsed);
  return parsed;
}

int calcInt(struct ParsedTime parsed) {
  int r = parsed.md1 * 1000 + parsed.md2 * 100 + parsed.sd1 * 10 + parsed.sd2;
  debugCalc("Changed int ", parsed, r);
  return r;
}

struct ParsedTime parseInt(int digit) {
  int sd2 = digit % 10;
  int sd1 = (digit / 10) % 10;
  int md2 = (digit / 100) % 10;
  int md1 = (digit / 1000);
  struct ParsedTime parsed = { md1, md2, sd1, sd2 };
  debugParse("Changed int ", digit, parsed);
  return parsed;
}

bool runtimeMode() {
  ParsedTime parsed = parseTime(runningTime);
  debugTime(parsed);
  timeMode(parsed);
  runningTime += mode;
  int detTime = runningTime;
  if (mode == 1) {
    detTime = setTime[currentMaxTime] - runningTime;
  }
  setSoundMapPerTime(detTime);
  bool itreset = false;
  if (mode == 1) {
    if (runningTime == setTime[currentMaxTime]) {
      currentMaxTime++;
      if (currentMaxTime > maxTimes) {
        currentMaxTime = 0;
      }
      runningTime = 0;
      itreset = (currentMaxTime == 0);
    }
  } else {
    if (runningTime < 0) {
      currentMaxTime++;
      if (currentMaxTime > maxTimes) {
        currentMaxTime = 0;
      }
      runningTime = setTime[currentMaxTime];
      itreset = (currentMaxTime == 0);
      ;
    }
  }
  if (itreset) {
    if (iterations != 0) {
      runningIteration--;
      clearLEDs();
      int r = 0;
      int g = brightness;
      int b = 0;
      if (runningIteration <= iterations / 2) {
        r = 0;
        g = 0;
        b = brightness;
      }
      if (runningIteration <= iterations / 4) {
        r = brightness;
        g = 0;
        b = 0;
      }
      showNumber(runningIteration, 0, r, g, b);
      showNumber(iterations, 3, brightness, brightness, brightness);
      strip.show();
      freqout(400, 450);
      freqout(600, 250);
      freqout(800, 250);
    }
  }
  Serial.print(runningIteration);
  Serial.print("/");
  Serial.print(iterations);
  Serial.print("x  ");
  Serial.print(runningTime);
  Serial.print("/");
  Serial.println(setTime[currentMaxTime]);
  return itreset;
}

void timeMode(ParsedTime parsed) {
  for (int i = 0; i < 10; i++) {
    deldel(i);
    del.show();
    char key = keypad_1.getKey();
    int a = (int)key;
    if (a >= 35) {
      Serial.print("Pressed: ");
      Serial.println(key);
      if (key == 'A') {
        pause();
      }
      if (key == '*') {
        setupStateGlobal = MAX_SUBSETUPS;
      }
    }
    playOrWaitNthTenOfSecond(i);
  }
  showTimeWithCorrectDeadline(parsed);
  unsigned long now = millis();
  Serial.println(now);
  long shouldWait = now - tStart;
  if (shouldWait > 0 && shouldWait < 1000000000) {
    if (shouldWait > 1000) {
      Serial.println("Not waiting! Took to much!");
    } else {
      Serial.print("wating: ");
      Serial.println(1000 - shouldWait);
      delay(1000 - shouldWait);
    }
  } else {
    delay(80);  //every 70 days go a bit wild
  }
}

void showTimeWithCorrectDeadline(ParsedTime parsed) {
  ParsedTime parsedOriginTime = parseTime(setTime[currentMaxTime]);
  //todo cal the real deadline for the 5959
  clearLEDs();
  showNumberWithDeadline(parsed.md1, 0, parsedOriginTime.md1);
  if (parsedOriginTime.md1 == 0) {
    showNumberWithDeadline(parsed.md2, 1, parsedOriginTime.md2);
  } else {
    showNumberWithDeadline(parsed.md2, 1, 9);
  }
  if (parsedOriginTime.md1 == 0 && parsedOriginTime.md2 == 0) {
    showNumberWithDeadline(parsed.sd1, 2, parsedOriginTime.sd1);
  } else {
    showNumberWithDeadline(parsed.sd1, 2, 5);
  }
  if (parsedOriginTime.md1 == 0 && parsedOriginTime.md2 == 0 && parsedOriginTime.sd1 == 0) {
    showNumberWithDeadline(parsed.sd2, 3, parsedOriginTime.sd2);
  } else {
    showNumberWithDeadline(parsed.sd2, 3, 9);
  }
  strip.show();
}

void showNumberWithDeadline(int value, int display, int max) {
  Serial.print(max);
  Serial.print(": ");
  int r = 0;
  int g = 0;
  int b = 0;
  if (max <= 0) {
    r = brightness;
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
  if (value == -65) {
    number = -65;
  }
  if (number == 0 || number == 2 || number == 6 || number == 8 || number == -65) {
    segmentN(display, 0, r, g, b);
  }
  if (number == 0 || number == 2 || number == 3 || number == 5 || number == 6 || number == 8) {
    segmentN(display, 1, r, g, b);
  }
  if (number == 0 || number == 1 || number == 3 || number == 4 || number == 5 || number == 6 || number == 7 || number == 8 || number == 9 || number == -1 || number == -65) {
    segmentN(display, 2, r, g, b);
  }
  if (number == 0 || number == 1 || number == 2 || number == 3 || number == 4 || number == 7 || number == 8 || number == 9 || number == -1 || number == -65) {
    segmentN(display, 3, r, g, b);
  }
  if (number == 0 || number == 2 || number == 3 || number == 5 || number == 7 || number == 8 || number == 9 || number == -65) {
    segmentN(display, 4, r, g, b);
  }
  if (number == 0 || number == 4 || number == 5 || number == 6 || number == 8 || number == 9 || number == -65) {
    segmentN(display, 5, r, g, b);
  }
  if (number == 2 || number == 3 || number == 4 || number == 5 || number == 6 || number == 8 || number == 9 || number == -1 || number == -65) {
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
  deldel(xtime, 0);
}

void deldel(int xtime, int mode) {
  clearDelDel();
  int x = xtime % 10;
  if (x == 9) {
    for (int i = 0; i < LED_COUNT_DEL - 1; i++) {
      del.setPixelColor(i, (brightness + 1) / 2, (brightness + 1) / 2, (brightness + 1) / 2);
      //Serial.print("-");
    }
  } else {
    if (mode == 0) {
      for (int i = 0; i < xtime; i++) {
        del.setPixelColor(i, brightness, 0, brightness);
        //Serial.print(mode);
      }
      for (int i = xtime; i < LED_COUNT_DEL - 1; i++) {
        del.setPixelColor(i, 0, brightness, brightness);
        //Serial.print("x");
      }
    } else {
      for (int i = 0; i < xtime; i++) {
        del.setPixelColor(i, brightness, 0, 0);
        //Serial.print(mode);
      }
      for (int i = xtime; i < LED_COUNT_DEL - 1; i++) {
        del.setPixelColor(i, 0, 0, brightness);
        //Serial.print("x");
      }
    }
  }
  //Serial.println("");
}

void clearDelDel() {
  for (int i = 0; i < LED_COUNT_DEL; i++) {
    del.setPixelColor(i, 0);
  }
}

void setSoundMap(int x) {
  for (int i = 0; i < LED_COUNT_DEL; i++) {
    soundMap[i] = x;
  }
}

void setSoundMapPerTime(int detTime) {
  setSoundMap(0);
  if (detTime == 0 && finalSound == 1) {
    setSoundMapPerTimeToFinal();
  } else if (detTime == 1 && threeTwoOneSound == 1) {
    setSoundMapPerTimeToOne();
  } else if (detTime == 2 && threeTwoOneSound == 1) {
    setSoundMapPerTimeToTwo();
  } else if (detTime == 3 && threeTwoOneSound == 1) {
    setSoundMapPerTimeToThree();
  } else if (detTime == setTime[currentMaxTime] / 2 && halfSound == 1) {
    setSoundMapPerTimeToTwoThirds();
  } else if (detTime == setTime[currentMaxTime] / 3 && thirdSound == 1) {
    setSoundMapPerTimeToOnehlaf();
  }
}

void setSoundMapPerTimeToFinal() {
  setSoundMap(1000);
}
void setSoundMapPerTimeToOne() {
  soundMap[8] = 900;
}
void setSoundMapPerTimeToTwo() {
  soundMap[7] = 800;
  soundMap[8] = 800;
}
void setSoundMapPerTimeToThree() {
  soundMap[6] = 700;
  soundMap[7] = 700;
  soundMap[8] = 700;
}
void setSoundMapPerTimeToTwoThirds() {
  soundMap[0] = 300;
  soundMap[6] = 300;
  soundMap[8] = 300;
}
void setSoundMapPerTimeToOnehlaf() {
  soundMap[0] = 400;
  soundMap[1] = 400;
  soundMap[5] = 400;
  soundMap[6] = 400;
  soundMap[8] = 400;
}

void clearLEDs() {
  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, 0);
  }
}

void resetMode() {
  runningIteration = iterations;
  currentMaxTime = 0;
  if (mode > 0) {
    runningTime = 0;
  }
  if (mode < 0) {
    runningTime = setTime[currentMaxTime];
  }
}

void debugTime(ParsedTime parsed) {
  debugTime(parsed, true);
}

void debugTime(ParsedTime parsed, bool nw) {
  Serial.print("time: ");
  Serial.print(parsed.md1);
  Serial.print(parsed.md2);
  Serial.print(":");
  Serial.print(parsed.sd1);
  if (nw) {
    Serial.println(parsed.sd2);
  } else {
    Serial.print(parsed.sd2);
  }
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

void debugParse(char title[], int orig, ParsedTime result) {
  Serial.print(title);
  Serial.print(orig);
  Serial.print(" to: ");
  debugTime(result);
}

void debugCalc(char title[], ParsedTime orig, int result) {
  Serial.print(title);
  debugTime(orig, false);
  Serial.print(" to: ");
  Serial.print(result);
}

void playOrWaitNthTenOfSecond(int i) {
  int standardDealy = 80;
  if (i < LED_COUNT_DEL) {
    if (soundMap[i] == 0) {
      delay(standardDealy);
    } else {
      freqout(soundMap[i], standardDealy);
    }
  } else {
    delay(standardDealy);
  }
}