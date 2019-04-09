#define PINforControl   D5 // pin connected to the small NeoPixels strip
#define NUMPIXELS1      12 // number of LEDs on strip
#define Potpin A0

#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS1, PINforControl, NEO_RGBW + NEO_KHZ800);

unsigned long patternInterval = 20 ;                            // time between steps in the pattern
unsigned long lastUpdate = 0 ;                                  // for millis() when last update occoured
unsigned long intervals [] = { 20, 100, 100, 100, 300, 100 } ;  // speed for each pattern - add here when adding more cases
int fadeStep = 0;                                               // step variable for fade function
int numberOfCases = 5;                                          // how many case statements or patterns you have
const byte button = D1;                                         // pin to connect button switch to between pin and ground

void setup() {
  strip.begin();                    // This initializes the NeoPixel library.
  wipe();                           // wipes the LED buffers
  pinMode(button, INPUT_PULLUP);    // change pattern button
}

void loop() {
  static int pattern = 4, lastReading;          // start with the fade function
  int reading = digitalRead(button);
  if(lastReading == HIGH && reading == LOW){
    pattern++ ;                                 // change pattern number
    fadeStep = 0;                               // reset the fade state variable
    if(pattern > numberOfCases) pattern = 0;    // wrap round if too big
    patternInterval = intervals[pattern];       // set speed for this pattern
    wipe();                                     // clear out the buffer
    delay(50);                                  // debounce delay
  }
  lastReading = reading;                        // save for next time

if(millis() - lastUpdate > patternInterval) updatePattern(pattern);
}

void  updatePattern(int pat){                         // call the pattern currently being created
  switch(pat) {
    case 0:
        colorWipe(strip.Color(  0,   0,   0));        // Black/off 
        break;
    case 1:
        rainbowCycle();
        break;
    case 2:
        theaterChaseRainbow();
        break;
    case 3:
         colorWipe(strip.Color(0, 190, 0));           // 
         break; 
    case 4:
         fade(0,255, 0,64, 0,0, 400);                 // fade 
         break;   
    case 5:
         rainbow();
         break;                  
  }
}

void fade(int redStartValue, int redEndValue, int greenStartValue, int greenEndValue, int blueStartValue, int blueEndValue, int totalSteps) {
static float redIncrement, greenIncrement, blueIncrement;
static float red, green, blue;
static boolean fadeUp = false;

if (fadeStep == 0){                                       // first step is to initialise the initial colour and increments
  red = redStartValue;
  green = greenStartValue;
  blue = blueStartValue;
  fadeUp = false;

  redIncrement = (float)(redEndValue - redStartValue) / (float)totalSteps;
  greenIncrement = (float)(greenEndValue - greenStartValue) / (float)totalSteps;
  blueIncrement = (float)(blueEndValue - blueStartValue) / (float)totalSteps;
  fadeStep = 1;                                         // next time the function is called start the fade
}
else {                                                  // all other steps make a new colour and display it
                                                        // make new colour
  red += redIncrement;
  green +=  greenIncrement;
  blue += blueIncrement;
 
                                                        // set up the pixel buffer
  for (int i = 0; i < strip.numPixels(); i++) {
  strip.setPixelColor(i, strip.Color((int)red,(int)green,(int)blue));
  }
                                                        // now display it
  strip.show();
fadeStep += 1;                                          // go on to next step
if(fadeStep >= totalSteps) {                            // finished fade
  if(fadeUp){                                           // finished fade up and back
     fadeStep = 0;
     return;                                            // so next call recalabrates the increments
  }
  // now fade back
  fadeUp = true;
  redIncrement = -redIncrement;
  greenIncrement = -greenIncrement;
  blueIncrement = -blueIncrement;
  fadeStep = 1;                                         // don't calculate the increments again but start at first change
}
 }
}

void rainbow() {                                        // modified from Adafruit example to make it a state machine
  int val;
  static uint16_t j=0;
    for(int i=0; i<strip.numPixels(); i++) {
      val = analogRead(Potpin);            // reads the value of the potentiometer (value between 0 and 1023)
      val = map(val, 0, 1023, 0, 255);
      strip.setBrightness(val);      
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
     j++;
  if(j >= 256) j=0;
  lastUpdate = millis();                                // time for next change to the display
 
}
void rainbowCycle() {  // modified from Adafruit example to make it a state machine
  int val;
  static uint16_t j=0;
    for(int i=0; i< strip.numPixels(); i++) {
      val = analogRead(Potpin);            // reads the value of the potentiometer (value between 0 and 1023)
      val = map(val, 0, 1023, 0, 255);
      strip.setBrightness(val);
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
  j++;
  if(j >= 256*5) j=0;
  lastUpdate = millis();                                // time for next change to the display
}

void theaterChaseRainbow() {                            // modified from Adafruit example to make it a state machine
  int val;
  static int j=0, q = 0;
  static boolean on = true;
     if(on){
            for (int i=0; i < strip.numPixels(); i=i+3) {
                val = analogRead(Potpin);            // reads the value of the potentiometer (value between 0 and 1023)
                val = map(val, 0, 1023, 0, 255);
                strip.setBrightness(val);                 
                strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
             }
     }
      else {
           for (int i=0; i < strip.numPixels(); i=i+3) {
               val = analogRead(Potpin);            // reads the value of the potentiometer (value between 0 and 1023)
               val = map(val, 0, 1023, 0, 255);
               strip.setBrightness(val);               
               strip.setPixelColor(i+q, 0);                       //turn every third pixel off
                 }
      }
     on = !on;                                                    // toggel pixelse on or off for next time
      strip.show();                                               // display
      q++;                                                        // update the q variable
      if(q >=3 ){                                                 // if it overflows reset it and update the J variable
        q=0;
        j++;
        if(j >= 256) j = 0;
      }
  lastUpdate = millis();                                          // time for next change to the display  
}

void colorWipe(uint32_t c) {                                      // modified from Adafruit example to make it a state machine
  int val;
  static int i =0;
    val = analogRead(Potpin);            // reads the value of the potentiometer (value between 0 and 1023)
    val = map(val, 0, 1023, 0, 255);
    strip.setBrightness(val);
    strip.setPixelColor(i, c);
    strip.show();
  i++;
  if(i >= strip.numPixels()){
    i = 0;
    wipe();                                                       // blank out strip
  }
  lastUpdate = millis();                                          // time for next change to the display
}


void wipe(){                                                      // clear all LEDs
  int val;
     for(int i=0;i<strip.numPixels();i++){
       val = analogRead(Potpin);            // reads the value of the potentiometer (value between 0 and 1023)
       val = map(val, 0, 1023, 0, 255);
       strip.setBrightness(val);       
       strip.setPixelColor(i, strip.Color(0,0,0));
       }
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
