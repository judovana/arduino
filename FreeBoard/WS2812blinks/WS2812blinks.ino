#include <Adafruit_NeoPixel.h>

#define PIN 4
//#define LED_COUNT 7 //0-6
//#define LED_COUNT 100
#define LED_COUNT 12
//#define LED_COUNT 704 //32*22 ??

// Create an instance of the Adafruit_NeoPixel class called "leds".
// That'll be what we refer to from here on...
//strip
//Adafruit_NeoPixel leds = Adafruit_NeoPixel(LED_COUNT, PIN, NEO_GRB + NEO_KHZ800);
//single led?
Adafruit_NeoPixel leds = Adafruit_NeoPixel(LED_COUNT, PIN, NEO_RGB + NEO_KHZ800);
//itherwise the order of rgb is puzzled

void setup()
{
  Serial.begin(9600);  
  leds.begin();  // Call this to start up the LED strip.
  clearLEDs();   // This function, defined below, turns all LEDs off...
  leds.show();   // ...but the LEDs don't actually update until you call this.
  for (int i=0; i<LED_COUNT; i++)
  {
    leds.setPixelColor(i, 5,5,5);
    delay(100);
    leds.show();   
  }
  delay(500);
  clearLEDs();
  leds.show();
}

 
boolean state = false;
void loop()
{
   while (true) 
    {
      delay(1000);
      //send eg mesage header in 4bytes?
        if (Serial.available() > 0) {
        setKNownNUmberOfLeds();
      }
    }
//  {
} 

//variant1 read array of known length
// reads 3 bytes per item
// reads all items before it yelds
void setKNownNUmberOfLeds()
{
  //buffering by byte[3]? meassure on big!
  //clearLEDs(); not necessary
  int i = 0;
  while (i<LED_COUNT)
  {
    byte data[] = {0,0,0};
    byte d = 0;
    while (d<3) {
    if (Serial.available() > 0) {
       data[d] = Serial.read();
       delay(1);//crucial!
       d++;
      }
    leds.setPixelColor(i, data[0], data[1], data[2]);
    }
    i++;
  }
  leds.show(); 
}

//variant2 clean, and ligth on  only arriving bubls
//needs clenup signal (6x 0?)
//then reads 6 bytes - three for index, three per r g b
//needs finished signal  6x 255?

// Sets all LEDs to off, but DOES NOT update the display;
// call leds.show() to actually turn them off after this.
void clearLEDs()
{
  for (int i=0; i<LED_COUNT; i++)
  {
    leds.setPixelColor(i, 0);
  }
}

