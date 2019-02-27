#define WITH_BLUETOOTH
#include <Adafruit_NeoPixel.h>

#ifdef WITH_BLUETOOTH
  #include <SoftwareSerial.h>  
  int bluetoothTx = 2;  // TX-O pin of bluetooth mate, Arduino D2
  int bluetoothRx = 3;  // RX-I pin of bluetooth mate, Arduino D3
  SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);
#endif

//contol of led
#define PIN 4
#define LED_COUNT 12

//reuse dby BT and serial 9600 is supersafe but slow
#define BAUD_RATE 28800
#ifdef WITH_BLUETOOTH
  #define BAUD_RATE_STRING "U,28800,N"
#endif

//strip
//Adafruit_NeoPixel leds = Adafruit_NeoPixel(LED_COUNT, PIN, NEO_GRB + NEO_KHZ800);
//single led?
Adafruit_NeoPixel leds = Adafruit_NeoPixel(LED_COUNT, PIN, NEO_RGB + NEO_KHZ800);
//itherwise the order of rgb is puzzled

void setup()
{
  Serial.begin(BAUD_RATE);  
  #ifdef WITH_BLUETOOTH
    bluetooth.begin(115200);  // The Bluetooth Mate defaults to 115200bps
    //By some doings, I someties casued to block in the config mode
    //bluetooth.print("$");  // Print three times individually
    //bluetooth.print("$");
    //bluetooth.print("$");  // Enter command mode
    //delay(100);  // Short delay, wait for the Mate to send back CMD
    //bluetooth.println(BAUD_RATE_STRING);  // Temporarily Change the baudrate to 9600, no parity
    // 115200 can be too fast at times for NewSoftSerial to relay the data reliably
    //bluetooth.begin(BAUD_RATE);  // start again
  #endif
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
        setKNownNUmberOfLedsSerial();
      }
      #ifdef WITH_BLUETOOTH
        if(bluetooth.available()) {
          setKNownNUmberOfLedsBT();
        }
      #endif
    }
//  {
} 

//variant1 read array of known length
// reads 3 bytes per item
// reads all items before it yelds
void setKNownNUmberOfLedsSerial()
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

#ifdef WITH_BLUETOOTH
//variant1 read array of known length
// reads 3 bytes per item
// reads all items before it yelds
void setKNownNUmberOfLedsBT()
{
  //buffering by byte[3]? meassure on big!
  //clearLEDs(); not necessary
  int i = 0;
  while (i<LED_COUNT)
  {
    byte data[] = {0,0,0};
    byte d = 0;
    while (d<3) {
    if (bluetooth.available()) {
       data[d] = bluetooth.read();
       delay(1);//crucial!
       d++;
      }
    leds.setPixelColor(i, data[0], data[1], data[2]);
    }
    i++;
  }
  leds.show(); 
}
#endif

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

