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
            e.l_arm.start = e.pos;
            e.r_arm.start = e.pos;
        } break;
        case EK_COUNT:
        default: ASSERT(false, "UNREACHABLE!");
    }

    return e;
}

void update_entity(Entity *e) {
    switch (e->kind) {
        case EK_BASE: {
            
        } break;
        case EK_BAT: {
            e->l_arm.start = e->pos;
            e->r_arm.start = e->pos;
            segment_end_to_start(&e->l_arm);
            segment_end_to_start(&e->r_arm);
        } break;
        case EK_COUNT:
        default: ASSERT(false, "UNREACHABLE!");
    }

    physics_update_entity(e);

}

void draw_entity(Entity *e, bool debug) {
    DrawCircleV(e->pos, (e->mass*1.5f) + 8.f, ColorFromHSV(e->mass * 100.f, 1.f, 1.f));
    if (debug) {
        Vector2 vel_line_end = Vector2Add(e->pos, e->vel);
        DrawLineV(e->pos, vel_line_end, BLUE);
    }

    switch (e->kind) {
        case EK_BASE: {
            
        } break;
        case EK_BAT: {
            draw_segment(&e->l_arm, debug);
            draw_segment(&e->r_arm, debug);
        } break;
        case EK_COUNT:
        default: ASSERT(false, "UNREACHABLE!");
    }
}
