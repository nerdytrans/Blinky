#pragma once
#include "LightingAlgorithm.h"

class RainbowCycle : public LightingAlgorithm {
public:
    RainbowCycle(tinyNeoPixel& strip, uint8_t wait);
    void run() override;

private:
    uint8_t _wait;
};
