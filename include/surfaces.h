#ifndef SURFACES_H_
#define SURFACES_H_

#include <surface.h>

typedef struct Surfaces Surfaces;

struct Surfaces {
    Surface *items;
    size_t count;
    size_t capacity;
};

#endif // SURFACES_H_
