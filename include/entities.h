#ifndef ENTITIES_H_
#define ENTITIES_H_

#include <stddef.h>
#include <entity.h>

typedef struct Entities Entities;

struct Entities {
    Entity *items;
    size_t count;
    size_t capacity;
};

#endif // ENTITIES_H_
