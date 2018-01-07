#ifndef __VEC2_H__
#define __VEC2_H__

#include <cmath>


#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif//M_PI


float degToRad(float deg);
float radToDeg(float rad);
float clamp(float value, float min, float max);


struct vec2_t {
    float x {0.0f};
    float y {0.0f};

    inline void set(const vec2_t &v) {
        x = v.x;
        y = v.y;
    }

    inline void fromAngle(float radians) {
        x = std::cos(radians);
        y = std::sin(radians);
    }

    inline void add(const vec2_t &v) {
        x += v.x;
        y += v.y;
    }

    inline void subtract(const vec2_t &v) {
        x -= v.x;
        y -= v.y;
    }

    inline void multiply(const vec2_t &v) {
        x *= v.x;
        y *= v.y;
    }

    inline void divide(const vec2_t &v) {
        x /= v.x;
        y /= v.y;
    }

    inline float distanceSquared() const {
        return (x * x) + (y * y);
    }

    inline float distance() const {
        return std::sqrt(distanceSquared());
    }

    inline void normalize() {
        float dist = distance();
        divide({dist, dist});
    }

    inline void setDistance(float distance) {
        normalize();
        multiply({distance, distance});
    }

    inline float angle() const {
        return std::atan2(y, x);
    }

};

#endif//__VEC2_H__
