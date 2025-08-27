#include <segment.h>
#include <raymath.h>

void segment_start_to_end(Segment *s) {
    Vector2 diff = Vector2Subtract(s->end, s->start);
    s->start = Vector2Subtract(s->end, Vector2Scale(Vector2Normalize(diff), s->length));
}

void segment_end_to_start(Segment *s) {
    Vector2 diff = Vector2Subtract(s->start, s->end);
    s->end = Vector2Subtract(s->start, Vector2Scale(Vector2Normalize(diff), s->length));
}

void draw_segment(Segment *s, bool debug) {
    DrawLineV(s->start, s->end, WHITE);
    if (debug) {
        DrawCircleV(s->start, 4.f, RED);
        DrawCircleV(s->end,   4.f, BLUE);
    }
}
