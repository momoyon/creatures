#ifndef PHYSICS_OBJECT_H_
#define PHYSICS_OBJECT_H_

#include <raylib.h>

typedef struct Physics_object Physics_object;

struct Physics_object {
    Vector2 pos;
    Vector2 vel;
    Vector2 acc;
    float friction;
    float mass;
    float elasticity; // 0.f ~ 1.f
    bool affected_by_gravity;

    bool collided_this_frame;
};

void update_physics_object(Physics_object *p, float dt);
void apply_force(Physics_object *p, Vector2 force);
float get_radius(Physics_object *p);

#endif // PHYSICS_OBJECT_H_
