#include <physics_object.h>
#include <raymath.h>
#include <common.h>

void update_physics_object(Physics_object *e) {
    float dt = GetFrameTime();
    float scaled_mass = e->mass * 0.001f;

    // If gravity flag set, add gravity as a force (Fg = m * g)
    if (e->affected_by_gravity) {
        e->acc = Vector2Add(e->acc, Vector2Scale(gravity(), scaled_mass));
        e->affected_by_gravity = false;
    }

    // Convert net force to acceleration: a = F / m (handle zero mass)
    Vector2 acceleration = (scaled_mass != 0.0f) ? Vector2Scale(e->acc, 1.0f / scaled_mass) : e->acc;

    e->vel = Vector2Add(e->vel, Vector2Scale(acceleration, dt));
    e->pos = Vector2Add(e->pos, Vector2Scale(e->vel, dt));

    e->acc = Vector2Zero();

    // Friction
    e->vel = Vector2Scale(e->vel, 1.0f - e->friction);
}

void apply_force(Physics_object *p, Vector2 force) {
    p->acc = Vector2Add(p->acc, force);
}

float get_radius(Physics_object *p) {
    return (p->mass*1.5f) + 8.f;
}
