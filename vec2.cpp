#include "vec2.h"


float degToRad(float deg) {
    return deg / 180.0f * float(M_PI);
}

float radToDeg(float rad) {
    return rad / float(M_PI) * 180.0f;
}

float clamp(float value, float min, float max) {
    return std::fmin(std::fmax(value, min), max);
}
