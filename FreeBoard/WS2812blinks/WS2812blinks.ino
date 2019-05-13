#define WITH_BLUETOOTH
#define WITH_SERIAL
#define WITH_HEADER

#include <Adafruit_NeoPixel.h>

#ifdef WITH_HEADER
  #define header_length 8
#endif

int READ_ALL_KNOWN_BYTES_ID = 1;
int READ_COLORS_WTH_COORD_ID = 2;
int SW_RESET = 0;


#ifdef WITH_BLUETOOTH
  #include <SoftwareSerial.h>  
  int bluetoothTx = 2;  // TX-O pin of bluetooth mate, Arduino D2
  int bluetoothRx = 3;  // RX-I pin of bluetooth mate, Arduino D3
  SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);
#endif

//contol of led
#define PIN 7
#define LED_COUNT 400                                                                                                                                             

//reuse dby BT and serial 9600 is supersafe but slow
//software serial can not work on different:(
#ifdef WITH_SERIAL
  #define BAUD_RATE_SERIAL 38400
#endif
#ifdef WITH_BLUETOOTH
  #define BAUD_RATE_BT 9600
  #define BAUD_RATE_STRING "U,9600,N"
#endif

//strip
//Adafruit_NeoPixel leds = Adafruit_NeoPixel(LED_COUNT, PIN, NEO_GRB + NEO_KHZ800);
//single leds or moonboard chain
Adafruit_NeoPixel leds = Adafruit_NeoPixel(LED_COUNT, PIN, NEO_RGB + NEO_KHZ800);
//itherwise the order of rgb is puzzled

void(* resetFunc) (void) = 0; //declare reset function @ address 0

void reset(){
  for (int i=0; i<LED_COUNT; i++)  {
    leds.setPixelColor(i, random(5,50), random(5,50), random(5,50));
  }
  leds.show();   
  delay(1000);
  clearAllBuffers();
  for (int i=0; i<LED_COUNT; i++)  {
    leds.setPixelColor(i, random(5,50), random(5,50), random(5,50));
  }
  leds.show();   
  delay(1000);
  resetFunc();
}

void setup()
{
  #ifdef WITH_SERIAL
    Serial.begin(BAUD_RATE_SERIAL);  
  #endif
  #ifdef WITH_BLUETOOTH
    bluetooth.begin(115200);  // The Bluetooth Mate defaults to 115200bps
    //By some doings, I someties casued to block in the config mode
    bluetooth.print("$");  // Print three times individually
    bluetooth.print("$");
    bluetooth.print("$");  // Enter command mode
    delay(100);  // Short delay, wait for the Mate to send back CMD
    bluetooth.println(BAUD_RATE_STRING);  // Temporarily Change the baudrate to 9600, no parity
    // 115200 can be too fast at times for NewSoftSerial to relay the data reliably
     bluetooth.begin(BAUD_RATE_BT);  // start again
  #endif
  leds.begin();  // Call this to start up the LED strip.
  clearLEDs();   // This function, defined below, turns all LEDs off...
  leds.show();   // ...but the LEDs don't actually update until you call this.
  for (int i=0; i<LED_COUNT; i++)
  {
    leds.setPixelColor(i, 5,5,5);
    delay(2000/LED_COUNT);
    leds.show();   
  }
  delay(500);
  clearAllBuffers();
}

 
void loop()
{

  long time = millis(); //for 3s jsut eat all buffers
  while (millis()-time<3000) {
    clearAllBuffers();
  }
  clearLEDs();
  leds.show();
  
   while (true) 
    {
      #ifdef WITH_BLUETOOTH
        if(bluetooth.available()) {
          #ifdef WITH_HEADER
            int header = decideHeaderBt();
          #else
            int header = READ_ALL_KNOWN_BYTES_ID;
          #endif
          if (header == READ_ALL_KNOWN_BYTES_ID) {
            setKNownNUmberOfLedsBT();
          } else if (header == READ_COLORS_WTH_COORD_ID) {
            readCoordAndColorUntilTrailingArrivesBT();
          } else if (header == SW_RESET) {
            reset();
          }
        }
      #endif
      #ifdef WITH_SERIAL
      if (Serial.available() > 0) {
        #ifdef WITH_HEADER
          int header = decideHeaderSerial();
        #else
          int header = READ_ALL_KNOWN_BYTES_ID;
        #endif
          if (header == READ_ALL_KNOWN_BYTES_ID) {
            setKNownNUmberOfLedsSerial();
          } else if (header == READ_COLORS_WTH_COORD_ID) {
            readCoordAndColorUntilTrailingArrivesSerial();
          } else if (header == SW_RESET) {
            reset();
          }
      }
      #endif
    }
}

 

#ifdef WITH_HEADER
void put(byte b, byte (*header)[header_length]){
 for( int idx = 0 ; idx < sizeof( *header )-1 ; ++idx ){
    (*header)[ idx ] = (*header)[ idx+1 ];
  }
  (*header)[header_length-1] = b;
}

int check(byte header[header_length]){
  if (
    header[0] == 255 &&
    header[1] == 0 &&
    header[2] == 255 &&
    header[3] == 255 &&
    header[4] == 255 &&
    header[5] == 0 &&
    header[6] == 112 &&
    header[7] == 124
    ) {
      return READ_COLORS_WTH_COORD_ID;
     }
  if (
    header[0] == 250 &&
    header[1] == 50 &&
    header[2] == 150 &&
    header[3] == 200 &&
    header[4] == 5 &&
    header[5] == 139 &&
    header[6] == 144 &&
    header[7] == 250
    ) {
      return READ_ALL_KNOWN_BYTES_ID;
     }
  if (
    header[0] == 255 &&
    header[1] == 255 &&
    header[2] == 255 &&
    header[3] == 255 &&
    header[4] == 255 &&
    header[5] == 255 &&
    header[6] == 255 &&
    header[7] == 255
    ) {
      return SW_RESET;//because BT sucks
     }
   return -1;
}
#endif

#ifdef WITH_SERIAL
#ifdef WITH_HEADER
int decideHeaderSerial(){
  byte header[] = {1,2,3,4,5,6,7,8};
  while(true) {
    delay(1);//crucial!
    if (Serial.available() > 0) {
       byte readed= Serial.read();
       //Serial.println(readed);
       put(readed, &header);
       int checkr = check(header);
       if (checkr >= 0) {
         return checkr;
       }
    }
  }
}
#endif
//variant2 read untill 5 zeroes trailing 
// set all dark
// reads 5 bytes per item
// coord1 coord2 r g b
//set coord1*255+coord2 to r g b
void readCoordAndColorUntilTrailingArrivesSerial()
{
    //buffering? meassure on big!
  clearLEDs();
  boolean done=false;
  while (true)
  {
    byte data[] = {0,0,0,0,0};
    byte d = 0;
    while (d<5 || done) {
    if (Serial.available() > 0) {
       data[d] = Serial.read();
       delay(1);//crucial!
       //Serial.println(data[d]);
       d++;
      }
      if (d>=4 && data[0] == data[1] && data[0] == data[2] && data[0] == data[3] && data[0] == data[4] && data[0] == 0){
        done=true;
        break;
      }
    }
    if (!done){
      leds.setPixelColor(data[0]*255+data[1], data[2], data[3], data[4]);
    } else {
      break;
    }
  }
  leds.show(); 
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
       //Serial.println(data[d]);
       d++;
      }
    leds.setPixelColor(i, data[0], data[1], data[2]);
    }
    i++;
  }
  leds.show(); 
}
#endif

#ifdef WITH_BLUETOOTH
#ifdef WITH_HEADER
int decideHeaderBt(){
  byte header[] = {1,2,3,4,5,6,7,8};
  while(true) {
    //delay(1);//crucial NOT to
    if (bluetooth.available()) {
       put(bluetooth.read(), &header);
       int checkr = check(header);
       if (checkr >= 0) {
         return checkr;
       }
    }
  }
}
#endif
//variant2 read untill 5 zeroes trailing 
// set all dark
// reads 5 bytes per item
// coord1 coord2 r g b
//set coord1*255+coord2 to r g b
void readCoordAndColorUntilTrailingArrivesBT()
{
    //buffering? meassure on big!
  clearLEDs();
  boolean done=false;
  while (true)
  {
    byte data[] = {0,0,0,0,0};
    byte d = 0;
    while (d<5 || done) {
    if (bluetooth.available()) {
       data[d] = bluetooth.read();
       //delay(1); //crucial to NOT delay
       //Serial.println(data[d]);
       d++;
      }
      if (d>=4 && data[0] == data[1] && data[0] == data[2] && data[0] == data[3] && data[0] == data[4] && data[0] == 0){
        done=true;
        break;
      }
    }
    if (!done){
      leds.setPixelColor(data[0]*255+data[1], data[2], data[3], data[4]);
    } else {
      break;
    }
  }
  leds.show(); 
}
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
       data[d] = bluetooth.read(); //reads char! 3 bytes by observing, four by specification??
       //delay(1); //crucial to NOT delay
       //Serial.println(data[d]);
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

void clearAllBuffers() {
      #ifdef WITH_BLUETOOTH
        while(bluetooth.available()) {
          bluetooth.read();  
        }
      #endif
      #ifdef WITH_SERIAL
        while (Serial.available() > 0) {
          Serial.read();
        }
      #endif
}
