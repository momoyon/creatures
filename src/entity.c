#include <entity.h>
#include <raymath.h>
#include <common.h>
#include <config.h>
#define COMMONLIB_REMOVE_PREFIX
#include <commonlib.h>

const char *entity_kind_as_str(Entity_kind ek) {
    switch (ek) {
        case EK_BASE: return "Base";
        case EK_BAT: return "Bat";
        case EK_COUNT:
        default: ASSERT(false, "UNREACHABLE!");
    }
    return "YOU SHOULD NO BE ABLE TO SEE THIS!!!!!!";
}

void physics_update_entity(Entity *e) {
    float dt = GetFrameTime();
    // Compute acceleration from accumulated forces (excluding gravity)
    Vector2 accel_from_forces = (e->mass != 0.0f) ? Vector2Scale(e->acc, 1.0f / e->mass) : e->acc;

    // Add gravity contribution scaled by mass so heavier objects fall faster (non-physical)
    Vector2 gravity_accel = Vector2Scale(gravity(), e->mass);

    // Integrate velocity (semi-implicit Euler)
    e->vel = Vector2Add(e->vel, Vector2Scale(accel_from_forces, dt));
    if (e->affected_by_gravity)
        e->vel = Vector2Add(e->vel, Vector2Scale(gravity_accel, dt));
    e->affected_by_gravity = false;

    // Integrate position
    e->pos = Vector2Add(e->pos, Vector2Scale(e->vel, dt));

    // Clear accumulated forces
    e->acc = Vector2Zero();

    // Friction
    e->vel = Vector2Scale(e->vel, 1.0f - e->friction);

    // Clamp max speed
    // float speed2 = Vector2LengthSqr(e->vel);
    // float max2 = e->max_speed * e->max_speed;
    // if (speed2 > max2) {
    //     e->vel = Vector2Scale(Vector2Normalize(e->vel), e->max_speed);
    // }

}

void apply_force_to_entity(Entity *e, Vector2 force) {
    e->acc = Vector2Add(e->acc, force);
}

Entity make_entity(Vector2 pos, Entity_kind kind) {
    Entity e = {
        .pos = pos,
        .kind = kind,
        .max_speed = ENTITY_DEFAULT_MAX_SPEED,
    };

    switch (e.kind) {
        case EK_BASE: {
            e.mass = 1.f;
        } break;
        case EK_BAT: {
        } break;
        case EK_COUNT:
        default: ASSERT(false, "UNREACHABLE!");
    }

    return e;
}

void draw_entity(Entity *e, bool debug) {
    if (debug) {
        DrawCircleV(e->pos, (e->mass*1.5f) + 8.f, ColorFromHSV(e->mass * 100.f, 1.f, 1.f));
        Vector2 vel_line_end = Vector2Add(e->pos, e->vel);
        DrawLineV(e->pos, vel_line_end, BLUE);
    }
}
