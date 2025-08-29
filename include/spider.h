#ifndef SPIDER_H_
#define SPIDER_H_

#include <raylib.h>
#include <leg.h>
#include <physics_object.h>

typedef struct Spider Spider;

struct Spider {
    Physics_object phy;

    float max_speed;

    bool dead;

    Leg l_leg;
    Leg r_leg;

    Physics_object l_foot;
    Physics_object r_foot;
};

Spider make_spider(Vector2 pos);
void draw_spider(Spider *s, bool debug);

void update_spider(Spider *s);

void control_spider(Spider *s);

#endif // SPIDER_H_
