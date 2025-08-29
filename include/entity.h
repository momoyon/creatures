#ifndef ENTITY_H_
#define ENTITY_H_

#include <raylib.h>
#include <physics_object.h>
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
    Physics_object phy;

    bool dead;

    Entity_kind kind;

    Segment head;
    Segment l_arm;
    Segment r_arm;

    Vector2 target;

    bool l_arm_moving_up;
    bool r_arm_moving_up;

    bool l_arm_flying;
    bool r_arm_flying;

    Vector2 l_up_pos;
    Vector2 r_up_pos;

    Vector2 l_flap_to_pos;
    Vector2 r_flap_to_pos;
};

void physics_update_entity(Entity *e);
void apply_force_to_entity(Entity *e, Vector2 force);
void apply_gravity_to_entity(Entity *e);
Entity make_entity(Vector2 pos, Entity_kind kind);
void update_entity(Entity *e);
void draw_entity(Entity *e, bool debug);
float entity_radius(Entity *e);

#endif // ENTITY_H_
