#ifndef ENTITY_H_
#define ENTITY_H_

#include <raylib.h>
#include <segment.h>

typedef struct Entity Entity;

#define ENTITY_MEMBERS \

typedef enum Entity_kind Entity_kind;

enum Entity_kind {
    EK_BASE,
    EK_BAT,
    EK_COUNT,
};

const char *entity_kind_as_str(Entity_kind ek);

struct Entity {
    Vector2 pos;
    Vector2 vel;
    Vector2 acc;
    float friction;
    float mass;
    bool affected_by_gravity;

    float max_speed;

    bool dead;

    Entity_kind kind;

    Segment l_arm;
    Segment r_arm;
};

void physics_update_entity(Entity *e);
void apply_force_to_entity(Entity *e, Vector2 force);
void apply_gravity_to_entity(Entity *e);
Entity make_entity(Vector2 pos, Entity_kind kind);
void update_entity(Entity *e);
void draw_entity(Entity *e, bool debug);

#endif // ENTITY_H_
