#include "RainbowCycle.h"

RainbowCycle::RainbowCycle(tinyNeoPixel& strip, uint8_t wait)
    : LightingAlgorithm(strip), _wait(wait) {}

// Slightly different, this makes the rainbow equally distributed throughout
void RainbowCycle::run() {
    uint16_t i, j;
    for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
        for (i = 0; i < _strip.numPixels(); i++) {
            _strip.setPixelColor(i, wheel(((i * 256 / _strip.numPixels()) + j) & 255));
        }
        _strip.show();
        delay(_wait);
    }
}
