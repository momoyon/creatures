#include <common.h>
#include <config.h>

Vector2 gravity(void) {
    Vector2 g = { 0, GRAVITY_FORCE * GetFrameTime() * 500.f};
    return g;
}
