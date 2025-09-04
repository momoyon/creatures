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
    s->phy.affected_by_gravity = true;
    float stand_speed = 1.f;
    if (s->l_foot.collided_this_frame) {
        can_stand |= true;
        stand_speed *= 2.f;
    }
    if (s->r_foot.collided_this_frame) {
        can_stand |= true;
        stand_speed *= 2.f;
    }

    if (can_stand) {
        Vector2 to_target = Vector2Scale(Vector2Normalize(Vector2Subtract(s->target, s->phy.pos)), stand_speed);
        s->phy.pos = Vector2Add(s->phy.pos, to_target);
        s->phy.affected_by_gravity = false;
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
