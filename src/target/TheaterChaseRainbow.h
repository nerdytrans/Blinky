#pragma once
#include "LightingAlgorithm.h"

class TheaterChaseRainbow : public LightingAlgorithm {
public:
    TheaterChaseRainbow(tinyNeoPixel& strip, uint8_t wait);
    void run() override;

private:
    uint8_t _wait;
};
