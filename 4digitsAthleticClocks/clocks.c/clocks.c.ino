  /*
 Controlling large 7-segment displays
 By: Nathan Seidle
 SparkFun Electronics
 Date: February 25th, 2015
 License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

 The large 7 segment displays can be controlled easily with a TPIC6C594 IC. This code demonstrates how to control
 four display.

 Here's how to hook up the Arduino pins to the Large Digit Driver

 Arduino pin 6 -> CLK (Green on the 6-pin cable)
 5 -> LAT (Blue)
 7 -> SER on the IN side (Yellow)
 5V -> 5V (Orange)
 Power Arduino with 12V and connect to Vin -> 12V (Red)
 GND -> GND (Black)

 There are two connectors on the Large Digit Driver. 'IN' is the input side that should be connected to
 your microcontroller (the Arduino). 'OUT' is the output side that should be connected to the 'IN' of addtional
 digits.

 Each display will use about 150mA with all segments and decimal point on.
*/

//GPIO declarations
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
byte segmentClock = 6;
byte segmentLatch = 5;
byte segmentData = 7;

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void setup()
{
  Serial.begin(9600);
  Serial.println("Large Digit Driver Example");

  pinMode(segmentClock, OUTPUT);
  pinMode(segmentData, OUTPUT);
  pinMode(segmentLatch, OUTPUT);

  digitalWrite(segmentClock, LOW);
  digitalWrite(segmentData, LOW);
  digitalWrite(segmentLatch, LOW);

  int x = 0;
  //x= 5200; //test
  while(1)
  {
    int second=x%60;
    int minute=x/60;
    int sd1=second/10;
    int sd2=second%10;
    int md1=minute/10;
    int md2=minute%10;
    //rember it is shifting, so first sent, is last in  appearing row
    showNumber(sd2, false);
    showNumber(sd1, false);
    showNumber(md2, true);
    showNumber(md1, false);
    delay(999);//a bit faster is better then a bit slower

    x++;
    //x %= 120; //test, Reset x after 2minutes
    x %= 5400; //Reset x after 90minutes

    Serial.println(x); //For debugging
  }
}

void loop()
{
   //Test pattern
   //showNumber(8, true);
   //showNumber(8, false);
   //showNumber(8, true);
   //showNumber(8, false);
}


//Takes a number and displays a number. Displays absolute value (no negatives)
void showNumber(float value, bool dot)
{
  int number = abs(value); //Remove negative signs and any decimals
  postNumber(number, dot);

  //Latch the current segment data
  digitalWrite(segmentLatch, LOW);
  digitalWrite(segmentLatch, HIGH); //Register moves storage register on the rising edge of RCK
}

//Given a number, or '-', shifts it out to the display
void postNumber(byte number, boolean decimal)
{
  //    -   A
  //   / / F B
  //    -   G
  //   / / E C
  //    -.  D DP

#define a  1<<0
#define b  1<<6
#define c  1<<5
#define d  1<<4
#define e  1<<3
#define f  1<<1
#define g  1<<2
#define dp 1<<7

  byte segments;

  switch (number)
  {
    case 1: segments = b | c; break;
    case 2: segments = a | b | d | e | g; break;
    case 3: segments = a | b | c | d | g; break;
    case 4: segments = f | g | b | c; break;
    case 5: segments = a | f | g | c | d; break;
    case 6: segments = a | f | g | e | c | d; break;
    case 7: segments = a | b | c; break;
    case 8: segments = a | b | c | d | e | f | g; break;
    case 9: segments = a | b | c | d | f | g; break;
    case 0: segments = a | b | c | d | e | f; break;
    case ' ': segments = 0; break;
    case 'c': segments = g | e | d; break;
    case '-': segments = g; break;
  }

  if (decimal) segments |= dp;

  //Clock these bits out to the drivers
  for (byte x = 0 ; x < 8 ; x++)
  {
    digitalWrite(segmentClock, LOW);
    digitalWrite(segmentData, segments & 1 << (7 - x));
    digitalWrite(segmentClock, HIGH); //Data transfers to the register on the rising edge of SRCK
  }
}


