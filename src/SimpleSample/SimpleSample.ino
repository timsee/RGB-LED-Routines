#define IS_RAINBOWDUINO 0 
#define IS_NEOPIXELS 1 
#define IS_SINGLE_LED 0 

/*!
 * 
 * ArduCor
 * Sample Sketch
 *
 * DESCRIPTION_PLACEHOLDER
 * 
 * Date: May 13, 2018
 * Github repository: http://www.github.com/timsee/ArduCor
 * License: MIT-License, LICENSE provided in root of git repo
 */
#include <ArduCor.h>

#if IS_NEOPIXELS
#include <Adafruit_NeoPixel.h>
#endif
#if IS_RAINBOWDUINO
#include <Rainbowduino.h>
#endif

//================================================================================
// Settings
//================================================================================

#if IS_NEOPIXELS
const byte CONTROL_PIN       = 6;      // pin used by NeoPixels library
const int  LED_COUNT         = 64;     // the number of LEDs in your Neopixel
#endif
#if IS_RAINBOWDUINO
const int  LED_COUNT         = 64;     // the number of LEDs controlled by your Rainbowduino
#endif
#if IS_SINGLE_LED
const byte R_PIN             = 6;
const byte G_PIN             = 5;
const byte B_PIN             = 4;
const int  LED_COUNT         = 1;      // Used by ArduCor to know you are controlling only one LED.
const byte IS_COMMON_ANODE   = 1;      // 0 if common cathode, 1 if common anode
#endif

//=======================
// ArduCor Setup
//=======================
// Library used to generate the RGB LED routines.
ArduCor routines = ArduCor(LED_COUNT);

#if IS_NEOPIXELS
//=======================
// Hardware Setup
//=======================

//NOTE: you may need to change the NEO_GRB or NEO_KHZ2800 for this sample to work with your lights.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(LED_COUNT, CONTROL_PIN, NEO_GRB + NEO_KHZ800);
#endif

//================================================================================
// Setup and Loop
//================================================================================

void setup()
{
  // initialize the hardware
#if IS_RAINBOWDUINO
  Rb.init();
#endif
#if IS_NEOPIXELS
  pixels.begin();
#endif
#if IS_SINGLE_LED
  pinMode(R_PIN, OUTPUT);
  pinMode(G_PIN, OUTPUT);
  pinMode(B_PIN, OUTPUT);
#endif

  // choose the default color for the single
  // color routines. This can be changed at any time.
  // and its set it to green in sample routines.
  // If its not set, it defaults to a faint orange.
  routines.setMainColor(0, 127, 0);

  // This sets how bright the LEDs shine in multi color routines.
  // It expects a value between 0 and 100. 0 means off, 100 
  // means full brightness.
  routines.brightness(50);
}

void loop()
{
  /*!
   * Each time a routines function is called, it updates the LEDs in the ArduCor library.
   * This loop continually updates the ArduCor library, applies the brightness settings 
   * from ArduCor, then updates the lighting hardware to show the new LED values.
   */
  
  // the simple sample shows both a single and a multi color routine, 
  // set this flag to switch between them.
  bool useMultiColorRoutine = false;
  // For the glimmer routine, a parameter determines how many LEDs get the "glimmer" effect.
  const byte glimmerPercent = 15;
  if (useMultiColorRoutine) {
      // if you're using the multi color routine, it'll fade between colors in the 
      // fire palette.
      routines.multiGlimmer(eFire,  
                            glimmerPercent); // percent of pixels to glimmer
  } else {
     routines.singleGlimmer(routines.mainColor().red, 
                            routines.mainColor().green, 
                            routines.mainColor().blue,
                            glimmerPercent); // percent of pixels to glimmer
  }
     
  routines.applyBrightness(); 
  // updates the LED hardware with the values in the routines object.
  updateLEDs(); 
  // delay for half a second
  delay(500);
}


#if IS_RAINBOWDUINO
void updateLEDs()
{
  int index = 0;
  for (int x = 0; x < 8; x++) {
    for (int y = 0; y < 8; y++)  {
      Rb.setPixelXY(x, y,
                    routines.red(index),
                    routines.green(index),
                    routines.blue(index));
      index++;
    }
  }
}
#endif
#if IS_NEOPIXELS
void updateLEDs()
{
  for (int x = 0; x < LED_COUNT; x++) {
    pixels.setPixelColor(x, pixels.Color(routines.red(x),
                                         routines.green(x),
                                         routines.blue(x)));
  }
  pixels.show();
}
#endif
#if IS_SINGLE_LED
void updateLEDs()
{
  if (IS_COMMON_ANODE) {
    analogWrite(R_PIN, 255 - routines.red(0));
    analogWrite(G_PIN, 255 - routines.green(0));
    analogWrite(B_PIN, 255 - routines.blue(0));
  }
  else {
    analogWrite(R_PIN, routines.red(0));
    analogWrite(G_PIN, routines.green(0));
    analogWrite(B_PIN, routines.blue(0));
  }
}
#endif
