#include <surface.h>
#include <raymath.h>
#include <engine.h>

void draw_surface(Surface *s, bool debug) {
    DrawLineV(s->start, s->end, WHITE);
    if (debug) {
        Vector2 diff = Vector2Subtract(s->end, s->start);
        Vector2 midpoint = Vector2Add(s->start, Vector2Scale(diff, 0.5f));
        Vector2 normal = Vector2Scale(Vector2Normalize(v2(-diff.y, diff.x)), 10.f);
        DrawLineV(midpoint, Vector2Add(midpoint, normal), GREEN);
    }
}
