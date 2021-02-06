#include <Adafruit_NeoPixel.h>

#define PIN     5
#define LED_COUNT 200
int brightness = 5;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  strip.begin();
  strip.show();
  int x = 0;
  //x= 5200; //test
  while (1)
  {
    int second = x % 60;
    int minute = x / 60;
    int sd1 = second / 10;
    int sd2 = second % 10;
    int md1 = minute / 10;
    int md2 = minute % 10;
    clearLEDs();
    //rember it is shifting, so first sent, is last in  appearing row
    showNumber(0, sd2, false);
    showNumber(1, sd1, false);
    showNumber(2, md2, true);
    showNumber(3, md1, false);
    strip.show();
    delay(999);//a bit faster is better then a bit slower

    x++;
    //x %= 120; //test, Reset x after 2minutes
    x %= 5400; //Reset x after 90minutes

    Serial.println(x); //For debugging
  }
}

//Takes a number and displays a number. Displays absolute value (no negatives)
void showNumber(int order, float value, bool dot)
{
  int number = abs(value); //Remove negative signs and any decimals
  strip.setPixelColor(order*2 , brightness*((number+0) % 6) , brightness*((number+1) % 6), brightness*((number+2) % 6)); 
  strip.setPixelColor(order*2+1 , brightness*((number+3) % 6), brightness*((number+4) % 6), brightness*((number+5) % 6)); 

}




void clearLEDs()
{
  for (int i = 0; i < LED_COUNT; i++)
  {
    strip.setPixelColor(i, 0);
  }
}
