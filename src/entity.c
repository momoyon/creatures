#include <entity.h>
#include <raymath.h>
#include <config.h>
#include <commonlib.h>

void physics_update_entity(Entity *e) {
    e->vel = Vector2Add(e->vel, e->acc);
    e->pos = Vector2Add(e->pos, Vector2Scale(e->vel, GetFrameTime()));
    e->acc = Vector2Scale(e->acc, 0.f);

    // Friction
    e->vel = Vector2Scale(e->vel, 1.f - e->friction);


    // Don't go over max_speed
    float speed = Vector2LengthSqr(e->vel);
    if (speed > e->max_speed*e->max_speed) {
        e->vel = Vector2Scale(Vector2Normalize(e->vel), e->max_speed);
    }
}

void apply_force_to_entity(Entity *e, Vector2 force) {
    e->acc = Vector2Add(e->acc, force);
}

Entity make_entity(Vector2 pos) {
    Entity e = {
        .pos = pos,
        .max_speed = ENTITY_DEFAULT_MAX_SPEED,
    };
    return e;
}

void draw_entity(Entity *e, bool debug) {
    if (debug) {
        DrawCircleV(e->pos, 8.f, RED);
        Vector2 vel_line_end = Vector2Add(e->pos, e->vel);
        DrawLineV(e->pos, vel_line_end, BLUE);
    }
}
