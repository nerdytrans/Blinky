#include <tinyNeoPixel.h>
#include "ColorWipe.h"
#include "TheaterChase.h"
#include "Rainbow.h"
#include "RainbowCycle.h"
#include "TheaterChaseRainbow.h"


#define PIN PIN_PB0

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
tinyNeoPixel strip = tinyNeoPixel(20, PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

void setup() {
  Serial.begin(9600);
  Serial.println("Blinky starting...");
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  // Fill along the length of the strip in various colors...
  Serial.println("ColorWipe: Red");
  ColorWipe(strip, strip.Color(255,   0,   0), 50).run(); // Red
  Serial.println("ColorWipe: Green");
  ColorWipe(strip, strip.Color(  0, 255,   0), 50).run(); // Green
  Serial.println("ColorWipe: Blue");
  ColorWipe(strip, strip.Color(  0,   0, 255), 50).run(); // Blue

  // Do a theater marquee effect in various colors...
  Serial.println("TheaterChase: White");
  TheaterChase(strip, strip.Color(127, 127, 127), 50).run(); // White, half brightness
  Serial.println("TheaterChase: Red");
  TheaterChase(strip, strip.Color(127,   0,   0), 50).run(); // Red, half brightness
  Serial.println("TheaterChase: Blue");
  TheaterChase(strip, strip.Color(  0,   0, 127), 50).run(); // Blue, half brightness

  Serial.println("Rainbow");
  Rainbow(strip, 10).run();             // Flowing rainbow cycle along the whole strip
  Serial.println("TheaterChaseRainbow");
  TheaterChaseRainbow(strip, 50).run(); // Rainbow-enhanced theaterChase variant
}
