#include "TheaterChase.h"

TheaterChase::TheaterChase(tinyNeoPixel& strip, uint32_t color, uint8_t wait)
    : LightingAlgorithm(strip), _color(color), _wait(wait) {}

// Theatre-style crawling lights.
void TheaterChase::run() {
    for (int j = 0; j < 10; j++) { // do 10 cycles of chasing
        for (int q = 0; q < 3; q++) {
            for (uint16_t i = 0; i < _strip.numPixels(); i = i + 3) {
                _strip.setPixelColor(i + q, _color); // turn every third pixel on
            }
            _strip.show();
            delay(_wait);
            for (uint16_t i = 0; i < _strip.numPixels(); i = i + 3) {
                _strip.setPixelColor(i + q, 0); // turn every third pixel off
            }
        }
    }
}
