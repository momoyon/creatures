#ifndef SURFACE_H_
#define SURFACE_H_
#include <raylib.h>

typedef struct Surface Surface;

struct Surface {
    Vector2 start, end;
    bool normal_clockwise; // NOTE: clockwise means the normal is the right side here: x <- start
                           //                                                          |
                           //                                                          |
                           //                                                          |
                           //                                                          |
                           //                                      (ANTICLOCKWISE) <---|---> (CLOCKWISE)
                           //                                                          |
                           //                                                          |
                           //                                                          |
                           //                                                          |
                           //                                                          x <-end
};

void draw_surface(Surface *s, bool debug);

#endif // SURFACE_H_
