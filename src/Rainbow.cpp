#include "Rainbow.h"

Rainbow::Rainbow(tinyNeoPixel& strip, uint8_t wait)
    : LightingAlgorithm(strip), _wait(wait) {}

void Rainbow::run() {
    uint16_t i, j;
    for (j = 0; j < 256; j++) {
        for (i = 0; i < _strip.numPixels(); i++) {
            _strip.setPixelColor(i, wheel((i + j) & 255));
        }
        _strip.show();
        delay(_wait);
    }
}
