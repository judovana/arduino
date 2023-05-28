#include <Keypad.h>


#define outpin 13   // audio out to speaker or amp


void freqout(int freq, int t)  // freq in hz, t in ms
{
  int hperiod;                               //calculate 1/2 period in us
  long cycles, i;
  pinMode(outpin, OUTPUT);                   // turn on output pin

  hperiod = (500000 / freq) - 7;             // subtract 7 us to make up for digitalWrite overhead

  cycles = ((long)freq * (long)t) / 1000;    // calculate cycles
 
  for (i=0; i<= cycles; i++){              // play note for t ms
    digitalWrite(outpin, HIGH);
    delayMicroseconds(hperiod);
    digitalWrite(outpin, LOW);
    delayMicroseconds(hperiod - 1);     // - 1 to make up for digitaWrite overhead
  }
pinMode(outpin, INPUT);                // shut off pin to avoid noise from other operations

}


#include <Keypad.h>


const byte rows = 4;                   
const byte cols = 4;                  
char keys[rows][cols] =                
{
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

      //17 for C0 - 7902 for h8
      //261 for c4- 493 for h4
      //https://muted.io/note-frequencies/
int freqs[rows][cols] =                
{
  {130,260,523,1046},
  {138,277,554,1108},
  {196,398,783,1567},
  {246,493,987,1975}
};

byte rowPins[rows] = {12,11,10,9};
byte colPins[cols]= {8,4,3,2};

Keypad keypad_1 = Keypad( makeKeymap(keys), rowPins, colPins, rows, cols ); 


void setup(){
    Serial.begin(9600);
}

void loop(){
  Serial.print("Time: ");
  long myTime = millis();
  Serial.println(myTime);

    char key = keypad_1.getKey();     
    int a = (int)key;
    bool played = false;
    if (a>=35) {
      Serial.println(key);
      Serial.println(a);
      for(int x = 0; x<rows; x++) {
        for(int y = 0; y<rows; y++) {
          if (key == keys[x][y]) {
            freqout(freqs[x][y], 100);
            played = true;
            break;
          }
        }
      }
    }
    if (!played) {
      delay(100);
    }
}