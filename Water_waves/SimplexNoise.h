#ifndef SIMPLEX_NOISE_H
#define SIMPLEX_NOISE_H

#include <Arduino.h>

class SimplexNoise {
public:
    // 2D Simplex noise
    static float noise(float xin, float yin);
};

#endif
