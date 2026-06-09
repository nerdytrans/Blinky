#pragma once
#include "LightingAlgorithm.h"

class ColorWipe : public LightingAlgorithm {
public:
    ColorWipe(tinyNeoPixel& strip, uint32_t color, uint8_t wait);
    void run() override;

private:
    uint32_t _color;
    uint8_t _wait;
};
