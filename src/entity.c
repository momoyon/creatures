#include <entity.h>
#include <raymath.h>
#include <common.h>
#include <config.h>
#define COMMONLIB_REMOVE_PREFIX
#include <commonlib.h>

#include <engine.h>

const char *entity_kind_as_str(Entity_kind ek) {
    switch (ek) {
        case EK_BASE: return "Base";
        case EK_BAT: return "Bat";
        case EK_COUNT:
        default: ASSERT(false, "UNREACHABLE!");
    }
    return "YOU SHOULD NO BE ABLE TO SEE THIS!!!!!!";
}

Entity make_entity(Vector2 pos, Entity_kind kind) {
    Entity e = {
        .kind = kind,
    };
    e.phy.pos = pos;

    switch (e.kind) {
        case EK_BASE: {
            e.phy.mass = 1.f;
        } break;
        case EK_BAT: {
            e.head.length = 10.f;
            e.head.start = e.phy.pos;
            e.l_arm.length = 24.f;
            e.r_arm.length = 24.f;
            e.l_arm.start = e.phy.pos;
            e.r_arm.start = e.phy.pos;
            e.l_arm_flying = true;
            e.flap_radius = 15.f;
        } break;
        case EK_COUNT:
        default: ASSERT(false, "UNREACHABLE!");
    }

    return e;
}

void update_entity(Entity *e, float dt) {
    switch (e->kind) {
        case EK_BASE: {
            
        } break;
        case EK_BAT: {
            e->l_arm.start = e->phy.pos;
            e->r_arm.start = e->phy.pos;
            e->head.start = e->phy.pos;

            e->head.end = e->target;
            segment_end_to_start(&e->head);

            // Fly
            Vector2 head = Vector2Subtract(e->head.end, e->head.start);
            Vector2 normal_of_head = v2(head.y, -head.x);
            e->l_up_pos = Vector2Add(e->head.end, Vector2Scale(Vector2Normalize(normal_of_head), e->l_arm.length));
            e->l_flap_to_pos = Vector2Add(e->phy.pos, Vector2Rotate(Vector2Scale(Vector2Normalize(normal_of_head), e->r_arm.length), DEG2RAD*-e->flap_radius));

            e->l_arm_flying = true;
            e->r_arm_flying = true;

            if (e->l_arm_flying) {
                // 1. Move arm up
                if (e->l_arm_moving_up) {
                    e->l_arm.end = Vector2MoveTowards(e->l_arm.end, e->l_up_pos, 1.f);

                    float dist_to_up_pos_sqr = Vector2LengthSqr(Vector2Subtract(e->l_arm.end, e->l_up_pos));
                    if (dist_to_up_pos_sqr <= 1.5f) {
                        e->l_arm_moving_up = false;
                    }
                    // log_info("L MOVING UP: %f", dist_to_up_pos_sqr);
                } else {
                    e->l_arm.end = Vector2MoveTowards(e->l_arm.end, e->l_flap_to_pos, 1.f);

                    float dist_to_up_flap_pos_sqr = Vector2LengthSqr(Vector2Subtract(e->l_arm.end, e->l_flap_to_pos));
                    if (dist_to_up_flap_pos_sqr <= 1.5f) {
                        e->l_arm_flying = false;
                        e->l_arm_moving_up = true;

                        Vector2 l_arm_flap_force = Vector2Rotate(Vector2Subtract(e->l_flap_to_pos, e->phy.pos), DEG2RAD*45.f);
                        float flap_force_mag = e->phy.mass * 2.f;

                        float dist_to_target_sqr = Vector2Length(Vector2Subtract(e->target, e->phy.pos));
                        if (dist_to_target_sqr <= 100.f) {
                            flap_force_mag *= 0.75f;
                        }
                        apply_force(&e->phy, Vector2Scale(l_arm_flap_force, flap_force_mag));
                    }
                    // log_info("L MOVING DOWN: %f", dist_to_up_flap_pos_sqr);
                }
            }

            normal_of_head = v2(-head.y, head.x);
            e->r_up_pos = Vector2Add(e->head.end, Vector2Scale(Vector2Normalize(normal_of_head), e->r_arm.length));
            e->r_flap_to_pos = Vector2Add(e->phy.pos, Vector2Rotate(Vector2Scale(Vector2Normalize(normal_of_head), e->r_arm.length), DEG2RAD*e->flap_radius));

            if (e->r_arm_flying) {
                // 1. Move arm up
                if (e->r_arm_moving_up) {
                    e->r_arm.end = Vector2MoveTowards(e->r_arm.end, e->r_up_pos, 1.f);

                    float dist_to_up_pos_sqr = Vector2LengthSqr(Vector2Subtract(e->r_arm.end, e->r_up_pos));
                    if (dist_to_up_pos_sqr <= 1.5f) {
                        e->r_arm_moving_up = false;
                    }
                    // log_info("R MOVING UP: %f", dist_to_up_pos_sqr);
                } else {
                    e->r_arm.end = Vector2MoveTowards(e->r_arm.end, e->r_flap_to_pos, 1.f);

                    float dist_to_up_flap_pos_sqr = Vector2LengthSqr(Vector2Subtract(e->r_arm.end, e->r_flap_to_pos));
                    if (dist_to_up_flap_pos_sqr <= 1.5f) {
                        e->r_arm_flying = false;
                        e->r_arm_moving_up = true;

                        Vector2 r_arm_flap_force = Vector2Rotate(Vector2Subtract(e->r_flap_to_pos, e->phy.pos), -DEG2RAD*45.f);
                        float flap_force_mag = e->phy.mass * 2.f;

                        float dist_to_target_sqr = Vector2Length(Vector2Subtract(e->target, e->phy.pos));
                        if (dist_to_target_sqr <= 100.f) {
                            flap_force_mag *= 0.75f;
                        }

                        apply_force(&e->phy, Vector2Scale(r_arm_flap_force, flap_force_mag));
                    }
                    // log_info("R MOVING DOWN: %f", dist_to_up_flap_pos_sqr);
                }
            }
            segment_end_to_start(&e->l_arm);
            segment_end_to_start(&e->r_arm);


            // Always follow the target
            // Vector2 force = Vector2Scale(Vector2Normalize(Vector2Subtract(e->target, e->phy.pos)), 1.f);
            // apply_force_to_entity(e, force);
        } break;
        case EK_COUNT:
        default: ASSERT(false, "UNREACHABLE!");
    }

    update_physics_object(&e->phy, dt);
}

void draw_entity(Entity *e, bool debug) {
    DrawCircleV(e->phy.pos, get_radius(&e->phy), ColorFromHSV(e->phy.mass * 100.f, 1.f, 1.f));
    if (debug) {
        Vector2 vel_line_end = Vector2Add(e->phy.pos, e->phy.vel);
        DrawLineV(e->phy.pos, vel_line_end, BLUE);
    }

    switch (e->kind) {
        case EK_BASE: {
            
        } break;
        case EK_BAT: {
            draw_segment(&e->l_arm, debug);
            draw_segment(&e->r_arm, debug);
            draw_segment(&e->head, debug);

            DrawCircleV(e->target, 2.f, GOLD);
            if (debug) {
                DrawCircleV(e->l_up_pos, 2.f, RED);
                DrawCircleV(e->r_up_pos, 2.f, RED);

                DrawCircleV(e->l_flap_to_pos, 2.f, GREEN);
                DrawCircleV(e->r_flap_to_pos, 2.f, GREEN);
            }
        } break;
        case EK_COUNT:
        default: ASSERT(false, "UNREACHABLE!");
    }
}

