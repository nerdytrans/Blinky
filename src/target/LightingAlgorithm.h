#pragma once
#include <tinyNeoPixel.h>

class LightingAlgorithm {
public:
    explicit LightingAlgorithm(tinyNeoPixel& strip);
    virtual ~LightingAlgorithm() = default;
    virtual void run() = 0;

protected:
    tinyNeoPixel& _strip;
    uint32_t wheel(byte wheelPos);
};
