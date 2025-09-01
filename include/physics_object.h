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
    bool affected_by_gravity;
};

void update_physics_object(Physics_object *p);
void apply_force(Physics_object *p, Vector2 force);
float get_radius(Physics_object *p);

#endif // PHYSICS_OBJECT_H_
