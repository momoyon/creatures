#ifndef ENTITY_H_
#define ENTITY_H_

#include <raylib.h>

typedef struct Entity Entity;

struct Entity {
    Vector2 pos;
    Vector2 vel;
    Vector2 acc;
    float friction;

    float max_speed;

    bool dead;
};

void physics_update_entity(Entity *e);
void apply_force_to_entity(Entity *e, Vector2 force);
Entity make_entity(Vector2 pos);
void draw_entity(Entity *e, bool debug);

#endif // ENTITY_H_
