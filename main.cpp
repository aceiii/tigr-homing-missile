#include <iostream>
#include <vector>

#include "tigr.h"
#include "vec2.h"
#include "missiles.h"
#include "random.h"


namespace {
    const int screen_width {800};
    const int screen_height {600};
    const int window_flags = TIGR_FIXED;
    const char *window_title = "Homing missiles!!11";

    Tigr *window {nullptr};
    Tigr *screen {nullptr};

    bool debug {false};

    int frame_time {0};
    int fps {0};

    int mouse_x {0};
    int mouse_y {0};
    int mouse_buttons {0};

    int screen_x {0};
    int screen_y {0};

    float screen_shake_time {0.0f};
    float screen_shake_life {0.0f};

    std::vector<missile_t> missiles;
    std::vector<missile_particle_t> missile_particles;
    std::vector<explosion_particle_t> explosion_particles;
}

bool init() {
    window = tigrWindow(screen_width, screen_height, window_title, window_flags);
    screen = tigrBitmap(screen_width, screen_height);

    return true;
}

void freeTigrPtr(Tigr *&ptr) {
    if (ptr) {
        tigrFree(ptr);
        ptr = nullptr;
    }
}

void shutdown() {
    freeTigrPtr(window);
    freeTigrPtr(screen);
}

void fireMissile() {
    const float life = 5.0f;
    const float velocity = 150.0f;

    const int center_x = screen_width / 2;
    const int center_y = screen_height / 2;

    const float rand_x = (float)randomInt(-center_x, center_x);
    const float rand_y = (float)randomInt(-center_y, center_y);
    const float rand_v = (float)randomInt(0, 20);
    const float rand_l = float((randomInt(0, 100) / 100.0f) * 5.0f);

    missile_t m;
    m.position.set({float(center_x), float(center_y)});
    m.life = life + rand_l;

    m.velocity.set({rand_x, rand_y});
    m.velocity.setDistance(velocity + rand_v);

    missiles.push_back(m);
}

bool mousePressed(int button) {
    static int mouse_down {0};

    if ((mouse_down & button) && !(mouse_buttons & button)) {
        mouse_down &= ~button;
    }
    else if (!(mouse_down & button) && (mouse_buttons & button)) {
        mouse_down |= button;
        return true;
    }

    return false;
}

void explode(const vec2_t &pos, float dt) {
    const int n = 16;
    const float a = float(M_PI / n * 2);
    const float a_offset = degToRad(float(randomInt(0, 360.0f / n)));
    const float pow = 80.0f;

    float r = 0.0f;
    for (int i = 0; i < n; i += 1) {
        const float p_offset = float(randomInt(0, 50));
        const float r_life = 0.9f + ((randomInt(0, 100) / 100.0f) * 0.5f);

        explosion_particle_t p;
        p.position.set(pos);
        p.velocity.fromAngle(r + a_offset);
        p.velocity.setDistance(pow + p_offset);
        p.life = r_life;

        explosion_particles.push_back(p);
        r += a;
    }
}

void shakeScreen(float dt) {
    screen_shake_life = 0.5f;
    screen_shake_time = 0.0f;
}

bool updateMissile(missile_t &m, float dt) {
    static const vec2_t drag {0.97f, 0.97f};
    static const vec2_t gravity {0.0f, -480.0f};
    static const float turn_radius = 200.0f;
    static const float dead_time = 2.0f;

    const int target_x = mouse_x - screen_x;
    const int target_y = mouse_y - screen_y;

    m.life -= dt;

    if (m.life < -dead_time) {
        explode(m.position, dt);
        return false;
    }

    if (m.life <= 0.0f) {
        m.velocity.multiply(drag);
        m.velocity.subtract({gravity.x * dt, gravity.y * dt});
        m.position.add({m.velocity.x * dt, m.velocity.y * dt});


        return true;
    }

    m.target.set({float(target_x), float(target_y)});
    m.position.add({m.velocity.x * dt, m.velocity.y * dt});

    vec2_t diff;
    diff.set(m.target);
    diff.subtract(m.position);

    if (diff.distanceSquared() <= 5.0f) {
        explode(m.position, dt);
        shakeScreen(dt);
        return false;
    }

    int r = randomInt(0, 4);
    if (r == 0) {
        const float r_time = 0.4f + ((randomInt(0, 100) / 100.0f) * 1.2f);

        missile_particle_t p;
        p.life = r_time;
        p.position.set(m.position);

        float particle_angle = radToDeg(m.velocity.angle());
        particle_angle += float(randomInt(-3, 3));

        p.velocity.fromAngle(degToRad(-particle_angle));
        p.velocity.setDistance(32.0f * dt);

        missile_particles.push_back(p);
    }

    const float target_angle = radToDeg(diff.angle());
    float current_angle = radToDeg(m.velocity.angle());
    float diff_angle = target_angle - current_angle;
    while (diff_angle < 0) {
        diff_angle += 360.0f;
    }

    if (diff_angle < 180.0f) {
        current_angle += std::min(turn_radius * dt, diff_angle);
    } else if (diff_angle > 180.0f) {
        current_angle -= std::min(turn_radius * dt, 360.0f - diff_angle);
    }

    vec2_t new_vel;
    new_vel.fromAngle(degToRad(current_angle));
    new_vel.setDistance(m.velocity.distance());

    m.velocity.set(new_vel);

    return true;
}

void updateMissiles(float dt) {
    auto it = std::remove_if(begin(missiles), end(missiles), [dt] (missile_t &m) {
        return !updateMissile(m, dt);
    });

    missiles.erase(it, end(missiles));

    std::sort(begin(missiles), end(missiles), [] (const missile_t &m1, const missile_t &m2) {
        return m1.position.x < m2.position.x;
    });
}

bool updateMissileParticle(missile_particle_t &p, float dt) {
    static const vec2_t force {0.0f, -200.0f};
    static const vec2_t drag {0.98f, 0.98f};

    p.time += dt;

    if (p.time >= p.life) {
        return false;
    }

    p.velocity.multiply(drag);
    p.velocity.add({force.x * dt, force.y * dt});
    p.position.add({p.velocity.x * dt, p.velocity.y * dt});

    return true;
}

void updateMissileParticles(float dt) {
    auto it = std::remove_if(begin(missile_particles), end(missile_particles),
        [dt] (missile_particle_t &p) {
            return !updateMissileParticle(p, dt);
        });

    missile_particles.erase(it, end(missile_particles));
}

bool updateExplosionParticle(explosion_particle_t &p, float dt) {
    static const vec2_t force {0.0f, 100.0f};
    static const vec2_t drag {0.99f, 0.99f};

    p.time += dt;

    if (p.time >= p.life) {
        return false;
    }

    p.velocity.multiply(drag);
    p.velocity.add({force.x * dt, force.y * dt});
    p.position.add({p.velocity.x * dt, p.velocity.y * dt});

    return true;
}

void updateExplosionParticles(float dt) {
    auto it = std::remove_if(begin(explosion_particles), end(explosion_particles),
        [dt] (explosion_particle_t &p) {
            return !updateExplosionParticle(p, dt);
        });

    explosion_particles.erase(it, end(explosion_particles));
}

bool processEvents() {
    if (tigrKeyDown(window, TK_ESCAPE)) {
        return false;
    }

    if (tigrKeyDown(window, 'D')) {
        debug = !debug;
    }

    return true;
}

void updateFPS(float dt) {
    frame_time = int(dt * 1000);
    fps = int(1.0f / dt);
}

void updateMouse(float dt) {
    tigrMouse(window, &mouse_x, &mouse_y, &mouse_buttons);

    if (mousePressed(1) || mouse_buttons & 4) {
        fireMissile();
    }

    if (mousePressed(2)) {
        const int count = 32;

        for (int i = 0; i < count; i += 1) {
            fireMissile();
        }
    }
}

void updateScreenShake(float dt) {
    static const float shake_amount = 5.0f;
    static const float shake_speed = 48.0f;

    if (screen_shake_time > screen_shake_life) {
        screen_x = 0;
        screen_y = 0;
        return;
    }

    screen_shake_time += dt;
    screen_y = int(std::sin(screen_shake_time * shake_speed) * shake_amount);
    screen_x = int(std::cos(screen_shake_life * shake_speed * 2) * shake_amount / 4.0f);
}

void update(float dt) {
    updateMouse(dt);
    updateScreenShake(dt);

    updateMissiles(dt);
    updateMissileParticles(dt);
    updateExplosionParticles(dt);
}

void drawMissile(const missile_t &m) {
    static const TPixel live_color = tigrRGB(255, 255, 0);
    static const TPixel dead_color = tigrRGB(37, 221, 245);
    static const TPixel line_color = tigrRGB(192, 192, 192);
    static const TPixel text_color = tigrRGB(255, 255, 255);

    static const int w = 4;
    static const int h = 4;
    static const int margin = 2;
    const int x = int(m.position.x);
    const int y = int(m.position.y);

    vec2_t v;
    v.set(m.velocity);
    v.multiply({-1.0f, -1.0f});
    v.setDistance(16.0f);

    const TPixel color = m.life < 0.0f ? dead_color : live_color;

    tigrFill(screen, x - (w / 2), y - (h / 2), w, h, color);
    tigrLine(screen, x, y, x + int(v.x), y + int(v.y), line_color);

    if (!debug) {
        return;
    }

    vec2_t t;
    t.set(m.target);
    t.subtract(m.position);
    t.subtract(m.velocity);

    char text[256];
    snprintf(text, sizeof(text), "p:(% 3d, % 3d)\nv:(% 3d, %3d)\na:% 4.2f\nta:% 4.2f",
             x, y,
             int(m.velocity.x), int(m.velocity.y),
             radToDeg(m.velocity.angle()),
             radToDeg(t.angle()));

    tigrPrint(screen, tfont, x + margin, y + margin, text_color, text);
}

void drawMissiles() {
    for (auto &m : missiles) {
        drawMissile(m);
    }
}

void drawMissileParticle(const missile_particle_t &p) {
    const TPixel color = tigrRGB(178, 178, 178);

    const int min = 2;
    const int max = 8;
    const int w = min + int((p.time / p.life) * (max - min));
    const int h = w;
    const int x = int(p.position.x);
    const int y = int(p.position.y);

    tigrRect(screen, x - (w / 2), y - (h / 2), w, h, color);
}

void drawMissleParticles() {
    for (auto &p : missile_particles) {
        drawMissileParticle(p);
    }
}

void drawExplosionParticle(const explosion_particle_t &p) {
    const TPixel color = tigrRGB(255, 255, 255);
    const float length = clamp(p.velocity.distance() / 200.0f, 0.0f, 1.0f);

    vec2_t d;
    d.set(p.velocity);
    d.multiply({-1.0f, -1.0f});
    d.setDistance(length * 12.0f);

    vec2_t v;
    v.set(p.position);
    v.add(d);

    const int x1 = int(p.position.x);
    const int y1 = int(p.position.y);
    const int x2 = int(v.x);
    const int y2 = int(v.y);

    tigrLine(screen, x1, y1, x2, y2, color);

    const int w = 2;
    const int h = w;
    const int x = x1 - (w / 2);
    const int y = y1 - (h / 2);

    tigrRect(screen, x, y, w, h, tigrRGB(190, 120, 0));
}

void drawExplosionParticles() {
    for (auto &p : explosion_particles) {
        drawExplosionParticle(p);
    }
}

void drawCrosshair() {
    static const TPixel color = tigrRGB(123, 175, 201);

    const int x = mouse_x - screen_x;
    const int y = mouse_y - screen_y;

    tigrLine(screen, x, 0, x, screen_height, color);
    tigrLine(screen, 0, y, screen_width, y, color);
}

void drawArrow() {
    const TPixel color = tigrRGB(213, 246, 221);

    const int center_x = screen_width / 2;
    const int center_y = screen_height / 2;

    const int target_x = mouse_x - screen_x;
    const int target_y = mouse_y - screen_y;

    vec2_t v {float(target_x - center_x), float(target_y - center_y)};
    v.setDistance(24.0f);

    tigrLine(screen, center_x, center_y, center_x + int(v.x), center_y + int(v.y), color);
}

void drawFPS() {
    const TPixel color = tigrRGB(255, 255, 255);
    const int margin = 8;

    char text[128];
    snprintf(text, sizeof(text), "% 4d ms/frame\n% 4d frames/sec", frame_time, fps);

    int width = tigrTextWidth(tfont, text);
    int height = tigrTextHeight(tfont, text);

    tigrPrint(window, tfont, screen_width - width - margin, screen_height - height - margin, color, text);
}

void drawParticleInfo() {
    const TPixel color = tigrRGB(255, 255, 255);
    const int margin = 8;
    const int m_count = (int)missiles.size();
    const int s_count = (int)missile_particles.size();
    const int p_count = (int)explosion_particles.size();

    char text[128];
    snprintf(text, sizeof(text),
             "% 4d missiles\n% 4d smoke\n% 4d sparks",
             m_count, s_count, p_count);

    int height = tigrTextHeight(tfont, text);

    tigrPrint(window, tfont, margin, screen_height - height - margin, color, text);
}

void drawMouseInfo() {
    const TPixel color = tigrRGB(255, 255, 255);
    const int margin = 8;

    const int mouse_window_x = mouse_x;
    const int mouse_window_y = mouse_y;

    const int center_x = screen_width / 2;
    const int center_y = screen_height / 2;


    vec2_t v {float(mouse_window_x - center_x), float(mouse_window_y - center_y)};
    const int angle = (int)radToDeg(v.angle());

    char text[128];
    snprintf(text, sizeof(text),
             "mouse position: (% 3d, %3d)\nmouse angle: % 3d deg\nbutton: % 3d",
             mouse_x, mouse_y, angle, mouse_buttons);

    int width = tigrTextWidth(tfont, text);

    tigrPrint(window, tfont, screen_width - width -margin, margin, color, text);
}

void drawGrid() {
    static const TPixel color = tigrRGB(148, 148, 148);
    static const int grid_size = 60;

    const int half_width = screen_width / 2;
    const int half_height = screen_height / 2;

    const int grid_x_count = 2 * ((half_width / grid_size) + 1);
    const int grid_y_count = 2 * ((half_height / grid_size) + 1);

    const int start_x = half_width - ((grid_x_count / 2) * grid_size);
    const int start_y = half_height - ((grid_y_count / 2) * grid_size);

    for (int j = 0; j < grid_y_count; j += 1) {
        const int y = start_y + (j * grid_size);

        for (int i = 0; i < grid_x_count; i += 1) {
            const int x = start_x + (i * grid_size);
            const int c = (i + j) % 2;

            if (c == 0) {
                tigrFill(screen, x, y, grid_size, grid_size, color);
            }
        }
    }
}

void render() {
    tigrClear(window, tigrRGB(64, 64, 64));
    tigrClear(screen, tigrRGB(127, 127, 127));

    drawGrid();

    drawMissleParticles();
    drawExplosionParticles();
    drawMissiles();

    drawCrosshair();
    drawArrow();

    tigrBlit(window, screen, screen_x, screen_y, 0, 0, screen_width, screen_width);

    drawFPS();
    drawParticleInfo();
    drawMouseInfo();

    tigrUpdate(window);
}

void run() {
    const double dt {0.01f};
    double accumulator {0.0f};
    while (!tigrClosed(window)) {
        if (!processEvents()) {
            break;
        }

        float time = tigrTime();
        accumulator += time;

        updateFPS(time);

        while (accumulator >= dt) {
            update(dt);
            accumulator -= dt;
        }

        render();
    }
}

int main() {
    if (!init()) {
        shutdown();
        return 1;
    }

    run();
    shutdown();

    return 0;
}
