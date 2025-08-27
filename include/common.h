#ifndef COMMON_H_
#define COMMON_H_

#include <raylib.h>

Vector2 gravity(void);
Vector2 warp_in_bounds(Vector2 p, Rectangle bounds);

#endif // COMMON_H_
