#pragma once
#include "LightingAlgorithm.h"

class Rainbow : public LightingAlgorithm {
public:
    Rainbow(tinyNeoPixel& strip, uint8_t wait);
    void run() override;

private:
    uint8_t _wait;
};
