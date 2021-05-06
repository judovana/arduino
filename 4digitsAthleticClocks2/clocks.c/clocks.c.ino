#include <Adafruit_NeoPixel.h>

#define PIN_NUMBERS     5
#define PIN_DELIMITER     7
#define LED_COUNT_PER_SRIP 60
#define LED_COUNT LED_COUNT_PER_SRIP*4
#define LED_COUNT_DEL 9
int brightness = 240;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, PIN_NUMBERS, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel del = Adafruit_NeoPixel(LED_COUNT_DEL, PIN_DELIMITER, NEO_GRB + NEO_KHZ800);

void setup() {
  del.begin();
  del.show();
  strip.begin();
  strip.show();
  int x = 0;
  //x= 5200; //test
  while (1)
  {
    for (int i = 0; i < 10; i++) {
      deldel(i);
      del.show() ;
      delay(99);
    }
    int second = x % 60;
    int minute = x / 60;
    int sd1 = second / 10;
    int sd2 = second % 10;
    int md1 = minute / 10;
    int md2 = minute % 10;
    clearLEDs();
    int r = 0;
    int g = 0;
    int b = 0;
    //0 1 2         3             4 5
    int sixDelimiter = 3;
    //0 1 2 3 4     5             6 7 8 9
    int tenDelimiter = 5;
    if (md1 < sixDelimiter) { //in 60 minutes, thsi behaves like  sd1 in 90 minutes, it would be like md1 and sd1
      g = brightness;
    } else if (md1 > sixDelimiter) {
      r = brightness;
    } else {
      b = brightness;
    }
    showNumber(md1, 0, r, g, b);
    r = g = b = 0;
    if (md2 < tenDelimiter) {
      g = brightness;
    } else if (md2 > tenDelimiter) {
      r = brightness;
    } else {
      b = brightness;
    }
    showNumber(md2, 1, r, g, b);
    r = g = b = 0;
    if (sd1 < sixDelimiter) { //this one is first letter of seconds in minute
      g = brightness;
    } else if (sd1 > sixDelimiter) {
      r = brightness;
    } else {
      b = brightness;
    }
    showNumber(sd1, 2, r, g, b);
    r = g = b = 0;
    if (sd2 < tenDelimiter) {
      g = brightness;
    } else if (sd2 > tenDelimiter) {
      r = brightness;
    } else {
      b = brightness;
    }
    showNumber(sd2, 3, r, g, b);
    strip.show();
    delay(9);//10*99+9 a bit faster is better then a bit slower
    x++;
    //x %= 120; //test, Reset x after 2minutes
    //x %= 5400; //Reset x after 90minutes
    x %= 3600; //Reset x after 90minutes

    Serial.println(x); //For debugging
  }
}

void showNumber(int value, int display, int r, int g, int b) {
  int number = abs(value); //Remove negative signs and any decimals
  if (number == 0 || number == 2 || number == 6 || number == 8  ) {
    segmentN(display, 0, r, g, b);
  }
  if (number == 0 || number == 2 || number == 3 || number == 5  || number == 6 || number == 8) {
    segmentN(display, 1, r, g, b);
  }
  if (number == 0 || number == 1 || number == 3 || number == 4 || number == 5  || number == 6 || number == 7 || number == 8 || number == 9) {
    segmentN(display, 2, r, g, b);
  }
  if (number == 0 || number == 1 || number == 2 || number == 3 || number == 4  || number == 7 || number == 8 || number == 9) {
    segmentN(display, 3, r, g, b);
  }
  if (number == 0 || number == 2 || number == 3 || number == 5 || number == 7 || number == 8 || number == 9) {
    segmentN(display, 4, r, g, b);
  }
  if (number == 0 || number == 4 || number == 5 || number == 6 || number == 8  || number == 9) {
    segmentN(display, 5, r, g, b);
  }
  if (number == 2 || number == 3 || number == 4 || number == 5 || number == 6 || number == 8 || number == 9) {
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
  for (int i = from; i <= to; i++)
  {
    strip.setPixelColor(i, r, g, b);
  }
}

void deldel(int xtime) {
  for (int i = 0; i < LED_COUNT_DEL; i++)  {
    del.setPixelColor(i, 0);
  }
  int x = xtime % 10;
  if (x == 9) {
    for (int i = 1; i < LED_COUNT_DEL - 1; i++)  {
      del.setPixelColor(i, brightness / 2, brightness / 2, brightness / 2);
    }
  } else {
    int color = x / 3;
    int r = 0;
    int g = 0;
    int b = 0;
    if (color == 0) {
      r = brightness; g = 0; b = 0;
    } else if (color == 1) {
      r = 0; g = 0; b = brightness;
    } else if (color == 2) {
      r = 0; g = brightness; b = 0;
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
      for (int i = 0; i < LED_COUNT_DEL; i++)  {
        del.setPixelColor(i, r, g, b);
      }
    }
  }
}


void clearLEDs()
{
  for (int i = 0; i < LED_COUNT; i++)  {
    strip.setPixelColor(i, 0);
  }
}
