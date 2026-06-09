#include "ColorWipe.h"

ColorWipe::ColorWipe(tinyNeoPixel& strip, uint32_t color, uint8_t wait)
    : LightingAlgorithm(strip), _color(color), _wait(wait) {}

// Fill the dots one after the other with a color
void ColorWipe::run() {
    for (uint16_t i = 0; i < _strip.numPixels(); i++) {
        _strip.setPixelColor(i, _color);
        _strip.show();
        delay(_wait);
    }
}
