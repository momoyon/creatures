#ifndef SEGMENT_H_
#define SEGMENT_H_

#include <raylib.h>

typedef struct Segment Segment;

struct Segment {
    Vector2 start, end;
    float length;
};

void segment_start_to_end(Segment *s);
void segment_end_to_start(Segment *s);

void draw_segment(Segment *s, bool debug);

#endif // SEGMENT_H_
