#include <spider.h>
#include <raymath.h>
#include <common.h>
#include <config.h>

void update_spider(Spider *s) {
    control_spider(s);

    s->l_leg.start = s->phy.pos;
    s->r_leg.start = s->phy.pos;

    update_physics_object(&s->phy);

    update_leg(&s->l_leg);
    update_leg(&s->r_leg);
}

Spider make_spider(Vector2 pos) {
    float x = 120.f;
    Spider s = {
        .phy.pos = pos,
        .phy.friction = 0.001f,
        .phy.mass = 1.f,
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
