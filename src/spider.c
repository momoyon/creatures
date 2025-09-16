#include <spider.h>
#include <raymath.h>
#include <common.h>
#include <config.h>
#include <engine.h>

void update_spider(Spider *s, float dt) {
    control_spider(s, dt);

    s->l_leg.end = s->l_foot.pos;
    s->r_leg.end = s->r_foot.pos;

    update_leg_start_to_end(&s->l_leg);
    update_leg_start_to_end(&s->r_leg);

    s->l_leg.start = s->phy.pos;
    s->r_leg.start = s->phy.pos;
    
    // Stand
    bool can_stand = false;
    Vector2 target = s->target;

    s->phy.affected_by_gravity = true;
    int n = 0;
    if (s->l_foot.collided_this_frame) {
        can_stand |= true;
        n++;
    }
    if (s->r_foot.collided_this_frame) {
        can_stand |= true;
        n++;
    }

    if (n==1) {
        // NOTE: One foot is on floor

    }

    if (can_stand) {
        Vector2 to_target = Vector2Normalize(Vector2Subtract(target, s->phy.pos));
        s->phy.pos = Vector2Add(s->phy.pos, to_target);
        s->phy.affected_by_gravity = false;
    }


    /// Walk @WIP
    if (can_stand) {
        // Update foot targets
        float dist2_l_foot_to_head = Vector2DistanceSqr(s->phy.pos, s->l_foot.pos);
        float dist2_r_foot_to_head = Vector2DistanceSqr(s->phy.pos, s->r_foot.pos);

        // log_debug("dist2_l_foot_to_head = %.2f", dist2_l_foot_to_head);
        // log_debug("dist2_r_foot_to_head = %.2f", dist2_r_foot_to_head);

        if (dist2_l_foot_to_head >= MAX_DIST2_FOOT) {
            s->l_foot_target.x = s->phy.pos.x - 10.f;
            s->l_foot_target.y = s->l_foot.pos.y;
        }

        if (dist2_r_foot_to_head >= MAX_DIST2_FOOT) {
            s->r_foot_target.x = s->phy.pos.x + 10.f;
            s->r_foot_target.y = s->r_foot.pos.y;
        }


        // Update foots
        float dist2_l_foot_to_target = Vector2Distance(s->l_foot_target, s->l_foot.pos);
        float dist2_r_foot_to_target = Vector2Distance(s->r_foot_target, s->r_foot.pos);

        if (dist2_l_foot_to_target >= 1.f) {
            if (s->l_foot_lerp_t == 0.f) {
                s->l_foot_from = s->l_foot.pos;
            } else {
                s->l_foot.pos = v2_parabolic_lerp(s->l_foot_from, s->l_foot_target, s->l_foot_lerp_t, 50.f, v2(0.f, -1.f));
            }
            s->l_foot_lerp_t += dt;
            if (s->l_foot_lerp_t >= 1.f) {
                s->l_foot_lerp_t = 0.f;
            }
        }

        if (dist2_r_foot_to_target >= 1.f) {
            if (s->r_foot_lerp_t == 0.f) {
                s->r_foot_from = s->r_foot.pos;
            } else {
                s->r_foot.pos = v2_parabolic_lerp(s->r_foot_from, s->r_foot_target, s->r_foot_lerp_t, 50.f, v2(0.f, -1.f));
            }
            s->r_foot_lerp_t += dt;
            if (s->r_foot_lerp_t >= 1.f) {
                s->r_foot_lerp_t = 0.f;
            }
        }
    }

    update_physics_object(&s->phy, dt);

    update_physics_object(&s->l_foot, dt);
    update_physics_object(&s->r_foot, dt);

    update_leg_end_to_start(&s->l_leg);
    update_leg_end_to_start(&s->r_leg);
}

Spider make_spider(Vector2 pos) {
    float x = 120.f;
    Spider s = {
        .phy.pos = pos,
        .phy.friction = 0.001f,
        .phy.mass = 1.f,
        .speed = 200.f,
        .l_foot.mass = 1.f,
        .r_foot.mass = 1.f,
        .l_foot.pos = Vector2Add(pos, v2(-10.f, 0)),
        .r_foot.pos = Vector2Add(pos, v2( 10.f, 0)),
        .l_leg = make_leg(pos, x),
        .r_leg = make_leg(pos, x),
        .stand_height = 120.f,
    };
    return s;
}

void draw_spider(Spider *s, bool debug) {
    float radius = get_radius(&s->phy);

    DrawCircleV(s->phy.pos, radius, WHITE);
    draw_leg(&s->l_leg, debug);
    draw_leg(&s->r_leg, debug);

    DrawCircleV(s->l_foot.pos, get_radius(&s->l_foot), GOLD);
    DrawCircleV(s->r_foot.pos, get_radius(&s->r_foot), PINK);

    if (debug) {
        DrawCircleV(s->l_foot_target, 2.f, GOLD);
        DrawCircleLinesV(s->l_foot_target, 2.f, WHITE);
        DrawLineV(s->l_foot.pos, s->l_foot_target, ColorAlpha(RED, 0.5f));

        DrawCircleV(s->r_foot_target, 2.f, PINK);
        DrawCircleLinesV(s->r_foot_target, 2.f, WHITE);
        DrawLineV(s->r_foot.pos, s->r_foot_target, ColorAlpha(RED, 0.5f));


        DrawCircleV(s->target, 4.f, RED);
        DrawLineV(s->phy.pos, s->target, ColorAlpha(RED, 0.5f));
    }
}

void control_spider(Spider *s, float dt) {
    Vector2 force = {0};
    if (IsKeyDown(KEY_LEFT)) {
        s->target.x -= s->speed * dt;
    }
    if (IsKeyDown(KEY_RIGHT)) {
        s->target.x += s->speed * dt;
    }

    if (IsKeyDown(KEY_UP)) {
        s->target.y -= s->speed * dt;
    }

    if (IsKeyDown(KEY_DOWN)) {
        s->target.y += s->speed * dt;
    }

    apply_force(&s->phy, force);
}
