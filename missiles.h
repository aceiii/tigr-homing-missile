#ifndef __MISSLES_H__
#define __MISSLES_H__

#include "vec2.h"


struct missile_t {
    vec2_t position {0.0f, 0.0f};
    vec2_t velocity {0.0f, 0.0f};
    vec2_t target {0.0f, 0.0f};
    float life {0.0f};
    float stage {0.0f};
    float stage_dir {-1.0f};
    float stage_max {0.5f};
    float stage_min {-0.02f};
};

struct missile_particle_t {
    vec2_t position {0.0f, 0.0f};
    vec2_t velocity {0.0f, 0.0f};
    float life {0.7f};
    float time {0.0f};
};

struct explosion_particle_t {
    vec2_t position {0.0f, 0.0f};
    vec2_t velocity {0.0f, 0.0f};
    float life {1.2f};
    float time {0.0f};
};


#endif//__MISSLES_H__
