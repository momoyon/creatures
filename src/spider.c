#include <spider.h>
#include <raymath.h>
#include <common.h>
#include <config.h>
#include <engine.h>

void update_spider(Spider *s) {
    control_spider(s);

    s->l_leg.end = s->l_foot.pos;
    s->r_leg.end = s->r_foot.pos;

    update_leg_start_to_end(&s->l_leg);
    update_leg_start_to_end(&s->r_leg);

    s->l_leg.start = s->phy.pos;
    s->r_leg.start = s->phy.pos;

    update_physics_object(&s->phy);

    update_physics_object(&s->l_foot);
    update_physics_object(&s->r_foot);

    update_leg_end_to_start(&s->l_leg);
    update_leg_end_to_start(&s->r_leg);
}

Spider make_spider(Vector2 pos) {
    float x = 120.f;
    Spider s = {
        .phy.pos = pos,
        .phy.friction = 0.001f,
        .phy.mass = 1.f,
        .l_foot.mass = 0.2f,
        .r_foot.mass = 0.2f,
        .l_foot.pos = Vector2Add(pos, v2(-10.f, 0)),
        .r_foot.pos = Vector2Add(pos, v2( 10.f, 0)),
        .l_leg = make_leg(pos, x),
        .r_leg = make_leg(pos, x),
    };
    return s;
}

void draw_spider(Spider *s, bool debug) {
    (void)debug;
    float radius =  (s->phy.mass*1.5f) + 8.f;

    DrawCircleV(s->phy.pos, radius, WHITE);
    draw_leg(&s->l_leg, debug);
    draw_leg(&s->r_leg, debug);

    DrawCircleV(s->l_foot.pos, 2.f, GOLD);
    DrawCircleV(s->r_foot.pos, 2.f, PINK);
}

void control_spider(Spider *s) {
    Vector2 force = {0};
    if (IsKeyDown(KEY_LEFT)) {
        force.x = -SPIDER_SPEED;
    }
    if (IsKeyDown(KEY_RIGHT)) {
        force.x = SPIDER_SPEED;
    }

    if (IsKeyDown(KEY_UP)) {
        force.y = -SPIDER_SPEED;
    }

    apply_force(&s->phy, force);
}
