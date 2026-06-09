#include "TheaterChaseRainbow.h"

TheaterChaseRainbow::TheaterChaseRainbow(tinyNeoPixel& strip, uint8_t wait)
    : LightingAlgorithm(strip), _wait(wait) {}

// Theatre-style crawling lights with rainbow effect
void TheaterChaseRainbow::run() {
    for (int j = 0; j < 256; j++) { // cycle all 256 colors in the wheel
        for (int q = 0; q < 3; q++) {
            for (uint16_t i = 0; i < _strip.numPixels(); i = i + 3) {
                _strip.setPixelColor(i + q, wheel((i + j) % 255)); // turn every third pixel on
            }
            _strip.show();
            delay(_wait);
            for (uint16_t i = 0; i < _strip.numPixels(); i = i + 3) {
                _strip.setPixelColor(i + q, 0); // turn every third pixel off
            }
        }
    }
}
