#ifndef LEG_H_
#define LEG_H_

#include <segment.h>

typedef struct Leg Leg;

struct Leg {
    Segment segments[3];
    Vector2 start, end;
};

Leg make_leg(Vector2 pos, float length);
void update_leg(Leg *l);
void draw_leg(Leg *l, bool debug);


#endif // LEG_H_
