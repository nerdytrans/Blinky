#include "LightingAlgorithm.h"

LightingAlgorithm::LightingAlgorithm(tinyNeoPixel& strip) : _strip(strip) {}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t LightingAlgorithm::wheel(byte wheelPos) {
    wheelPos = 255 - wheelPos;
    if (wheelPos < 85) {
        return _strip.Color(255 - wheelPos * 3, 0, wheelPos * 3);
    }
    if (wheelPos < 170) {
        wheelPos -= 85;
        return _strip.Color(0, wheelPos * 3, 255 - wheelPos * 3);
    }
    wheelPos -= 170;
    return _strip.Color(wheelPos * 3, 255 - wheelPos * 3, 0);
}
