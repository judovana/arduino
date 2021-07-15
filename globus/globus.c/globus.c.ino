#include <Adafruit_NeoPixel.h>

#define PIN_NUMBERS     5

Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, PIN_NUMBERS, NEO_RGB + NEO_KHZ800);

void setup() {
  strip.begin();
  strip.setPixelColor(0, 50, 30, 0);
  strip.show();
}

void loop()
{
}
