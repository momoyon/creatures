#include <common.h>
#include <config.h>

Vector2 gravity(void) {
    Vector2 g = { 0, GRAVITY_FORCE * GetFrameTime() * 500.f};
    return g;
}

Vector2 warp_in_bounds(Vector2 p, Rectangle bounds) {
    if (p.x < bounds.x) {
        p.x = bounds.x + bounds.width;
    }
    if (p.y < bounds.y) {
        p.y = bounds.y + bounds.height;
    }
    if (p.x > bounds.x + bounds.width) {
        p.x = bounds.x;
    }
    if (p.y > bounds.y + bounds.height) {
        p.y = bounds.y;
    }

    return p;
}
