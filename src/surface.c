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

bool surface_resolve_with_physics_object(Surface *s, Physics_object phy) {
    Vector2 diff = Vector2Subtract(s->end, s->start);

    Vector2 normal = Vector2Normalize(v2(-diff.y, diff.x));
    float D = signed_2d_cross_point_line(s->start, s->end, e->phy.pos);
    bool was_on_left = D > radius;

    if (!was_on_left) {
        normal.x = diff.y;
        normal.y = -diff.x;
    }

    Physics_object next_phy = e->phy;
    update_physics_object(&next_phy);

    if (coll_detect_circle_line_segment(surf->start, surf->end, next_phy.pos, radius, NULL, NULL)) {
        D = signed_2d_cross_point_line(surf->start, surf->end, next_phy.pos);
        bool will_be_on_left = D < -radius;

        if (was_on_left != will_be_on_left) {
            Vector2 reflect_force = Vector2Reflect(Vector2Normalize(e->phy.vel), normal);
            float vel_mag = Vector2Length(e->phy.vel);
            e->phy.vel = Vector2Scale(Vector2Normalize(reflect_force), vel_mag * 0.9f);

            // log_debug("Reflect force: %f, %f vs Velocity: %f, %f", reflect_force.x, reflect_force.y, e->phy.vel.x, e->phy.vel.y);
        }
    }
}
