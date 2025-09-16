#ifndef SPIDER_H_
#define SPIDER_H_

#include <raylib.h>
#include <leg.h>
#include <physics_object.h>

typedef struct Spider Spider;

struct Spider {
    Physics_object phy;

    float speed;

    bool dead;

    Leg l_leg;
    Leg r_leg;

    Physics_object l_foot;
    Physics_object r_foot;

    Vector2 l_foot_from;
    Vector2 r_foot_from;

    Vector2 l_foot_target;
    Vector2 r_foot_target;

    float l_foot_lerp_t;
    float r_foot_lerp_t;

    Vector2 target;

    float stand_height;
};

Spider make_spider(Vector2 pos);
void draw_spider(Spider *s, bool debug);

void update_spider(Spider *s, float dt);
void control_spider(Spider *s, float dt);

#endif // SPIDER_H_
