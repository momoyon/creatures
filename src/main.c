#include <config.h>
#include <common.h>
#include <entities.h>
#include <segment.h>
#include <leg.h>
#include <spider.h>
#include <physics_object.h>
#include <surfaces.h>

#define ENGINE_IMPLEMENTATION
#include <engine.h>

#define COMMONLIB_REMOVE_PREFIX
#define COMMONLIB_IMPLEMENTATION
#include <commonlib.h>

#define STB_DS_IMPLEMENTATION
#include <stb_ds.h>

#include <raylib.h>

#define DRAW_INFO(font_size, color, fmt, ...) do {\
        const char *str = arena_alloc_str(temp_arena, fmt, __VA_ARGS__);\
        draw_text(GetFontDefault(), str, v2(2, y), font_size, color);\
        y += font_size + 2;\
    } while (0)

#define log_info_a(console, fmt, ...) log_info_console((console), fmt, __VA_ARGS__); log_info(fmt, __VA_ARGS__)

#define FACTOR 105
#define SCREEN_WIDTH (16 * FACTOR)
#define SCREEN_HEIGHT (9 * FACTOR)
#define SCREEN_SCALE 0.25

typedef struct {
    Vector2 pos;
    float r;
    float spawn_r;
} Radius;


typedef struct {
    Radius *items;
    size_t count;
    size_t capacity;
} Radiuses;

typedef enum {
    MODE_NORMAL,
    MODE_EDIT,
    MODE_COUNT,
} Mode;

void change_mode(Mode* last_mode, Mode* current_mode, bool *changed_mode_this_frame, Mode mode_to)
{
    *last_mode = *current_mode;
    *current_mode = mode_to;
    *changed_mode_this_frame = true;
}

#define CHANGE_MODE(mode_to)                               \
    do {                                                   \
        change_mode(&last_mode, &current_mode, &changed_mode_this_frame, (mode_to)); \
    } while (0);

const char* mode_as_str(const Mode m)
{
    switch (m) {
    case MODE_NORMAL:
        return "Normal";
    case MODE_EDIT:
        return "Edit";
    case MODE_COUNT:
    default:
        ASSERT(false, "UNREACHABLE!");
    }
}

void add_bound_as_surfaces(Rectangle bounds, Surfaces *surfaces) {
    Vector2 tl = v2(bounds.x, bounds.y);
    Vector2 tr = v2(bounds.x + bounds.width, bounds.y);
    Vector2 br = v2(bounds.x + bounds.width, bounds.y + bounds.height);
    Vector2 bl = v2(bounds.x, bounds.y + bounds.height);

    Surface s = {0};

    // Left
    s.start = bl;
    s.end = tl;
    darr_append(*surfaces, s);

    // Top
    s.start = tl;
    s.end = tr;
    darr_append(*surfaces, s);
    
    // Right
    s.start = tr;
    s.end = br;
    darr_append(*surfaces, s);
    
    // Bottom
    s.start = br;
    s.end = bl;
    darr_append(*surfaces, s);
}

int main(void)
{
    int width = 0;
    int height = 0;

#if defined(DEBUG)
    bool debug_draw = true;
#else
    bool debug_draw = false;
#endif // defined(DEBUG)

    SetConfigFlags(FLAG_VSYNC_HINT);

    const char* window_name = "Creatures";
    RenderTexture2D ren_tex = init_window(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_SCALE, window_name,
        &width, &height);
    SetExitKey(0);

    // Font font = GetFontDefault();
    //
    Mode last_mode = MODE_NORMAL;
    Mode current_mode = MODE_NORMAL;
    bool changed_mode_this_frame = false;

    Arena temp_arena = arena_make(0);

    Camera2D cam = {
        .zoom = 1.0,
        .offset = CLITERAL(Vector2) { width / 2, height / 2 },
    };
    float cam_speed = 180.f;
    // Vector2 mpos_from = { 0 };

    bool quit = false;

    Entities entities = {0};
    Spider spider = make_spider(v2xx(0));

    spider.l_foot_target.y = -5.f;
    spider.r_foot_target.y = -5.f;
    spider.l_foot.pos.y    = -5.f;
    spider.r_foot.pos.y    = -5.f;
    spider.target.y        = -5.f;
    spider.phy.pos.y       = -5.f;

    // Edit
    Entity_kind edit_entity_kind = EK_BASE;
    
    /// @DEBUG
    bool do_apply_force = false;
    bool follow_mouse = false;
    float mass = 1.f;
    float elasticity = 0.5f;
    float force_magnitude = 1.f;
    /// 

    Rectangle bounds = {
        .x = -width*1.f,
        .y = -height*2.f,
        .width = width*2.f,
        .height = height*2.f,
    };

    Surfaces surfaces = {0};

    Radiuses radiuses = {0};
    Alarm radius_alarm = { .alarm_time = 0.25f };

    add_bound_as_surfaces(bounds, &surfaces);

    static double accumulator = 0.0;
    const float FIXED_STEP = 1.0f / 360.0f; // or 1/60
    const float MAX_ACCUM = 0.25f;          // cap accumulated time to avoid spiral of death
    const float MAX_FRAME_DT = 0.25f;


    /// @DEBUG
    Vector2 target = {10, 0};
    Vector2 t_pos = {0};
    Vector2 start = {0};
    float t = 0.f;
    ///

    while (!quit && !WindowShouldClose()) {
        arena_reset(&temp_arena);
        const char* title_str = arena_alloc_str(temp_arena, "%s | %d FPS", window_name, GetFPS());

        float frame_dt = GetFrameTime();
        if (frame_dt > MAX_FRAME_DT) frame_dt = MAX_FRAME_DT;
        accumulator += frame_dt;
        if (accumulator > MAX_ACCUM) accumulator = MAX_ACCUM;

        SetWindowTitle(title_str);

        changed_mode_this_frame = false;

        BeginDrawing();
        Vector2 m = get_mpos_scaled(SCREEN_SCALE);
        Vector2 m_world = GetScreenToWorld2D(m, cam);

        // Input
        if (IsKeyPressed(KEY_GRAVE)) {
            debug_draw = !debug_draw;
        }

        if (IsKeyPressed(KEY_TAB)) {
            CHANGE_MODE((current_mode + 1) % MODE_COUNT);
        }

        if (IsKeyDown(KEY_A)) {
            cam.target.x -= frame_dt * cam_speed * (IsKeyDown(KEY_LEFT_SHIFT) ? 2.f : 1.f);
        }
        if (IsKeyDown(KEY_D)) {
            cam.target.x += frame_dt * cam_speed * (IsKeyDown(KEY_LEFT_SHIFT) ? 2.f : 1.f);
        }
        if (IsKeyDown(KEY_W)) {
            cam.target.y -= frame_dt * cam_speed * (IsKeyDown(KEY_LEFT_SHIFT) ? 2.f : 1.f);
        }
        if (IsKeyDown(KEY_S)) {
            cam.target.y += frame_dt * cam_speed * (IsKeyDown(KEY_LEFT_SHIFT) ? 2.f : 1.f);
        }

        if (IsKeyDown(KEY_E)) {
            cam.zoom += frame_dt;
        }

        if (IsKeyDown(KEY_Q)) {
            cam.zoom -= frame_dt;
            if (cam.zoom <= 0.001f) cam.zoom = 0.001f;
        }
        
        if (IsKeyPressed(KEY_ZERO)) cam.zoom = 1.f;

        /// @DEBUG
        if (IsKeyDown(KEY_ONE)) {
            mass -= frame_dt;
            if (mass <= 0.001f) mass = 0.001f;
        }

        if (IsKeyDown(KEY_TWO)) {
            mass += frame_dt;
        }

        /// @DEBUG
        if (IsKeyDown(KEY_LEFT_CONTROL)) {
            if (IsKeyDown(KEY_LEFT)) {
                t -= frame_dt;
                if (t < 0.f) t = 0.f;
            }
            if (IsKeyDown(KEY_RIGHT)) {
                t += frame_dt;
                if (t > 1.f) t = 1.f;
            }

        }
        ///

        if (IsKeyDown(KEY_THREE)) {
            elasticity -= frame_dt;
            if (elasticity <= 0.f) mass = 0.f;
        }

        if (IsKeyDown(KEY_FOUR)) {
            elasticity += frame_dt;
            if (elasticity > 1.f) elasticity = 1.f;
        }

        if (IsKeyPressed(KEY_SPACE)) {
            Entity e = make_entity(m_world, edit_entity_kind);
            e.phy.friction = 0.001f;
            e.phy.mass = mass;
            e.phy.elasticity = elasticity;

            darr_append(entities, e);
        }
        do_apply_force = IsKeyDown(KEY_X);
        force_magnitude = IsKeyDown(KEY_LEFT_SHIFT) ? 2.f : 1.f;
        if (do_apply_force && on_alarm(&radius_alarm, frame_dt)) {
            float R = 100.f;
            Radius r = {
                .pos = m_world,
                .spawn_r = R,
                .r = R,
            };
            darr_append(radiuses, r);
        }
        follow_mouse = IsKeyDown(KEY_C);
        ///

        /// @DEBUG
        if (IsKeyDown(KEY_T)) {
            spider.r_foot.pos = m_world;
        }
        if (IsKeyDown(KEY_R)) {
            spider.l_foot.pos = m_world;
        }
        ///

        // Mode-specific Input
        switch (current_mode) {
        case MODE_NORMAL: {
        } break;
        case MODE_EDIT: {
            if (IsKeyPressed(KEY_K)) {
                edit_entity_kind = (edit_entity_kind + 1) % EK_COUNT;
            }
        } break;
        case MODE_COUNT:
        default:
            ASSERT(false, "UNREACHABLE!");
        }

        BeginTextureMode(ren_tex);
        ClearBackground(BLACK);

        // Update

        /// @DEBUG
        target = m_world;
        t_pos = v2_parabolic_lerp(start, target, t, 100.f, v2(0.f, -1.f));
        ///

        // Mode-specific Update
        switch (current_mode) {
        case MODE_NORMAL: {
            while (accumulator >= FIXED_STEP) {
                float dt = FIXED_STEP;
                // Update entities 
                for (size_t i = 0; i < entities.count; ++i) {
                    Entity *e = &entities.items[i];
                    if (e->dead) continue;
                    e->phy.affected_by_gravity = true;

                    if (do_apply_force) {
                        Vector2 force = Vector2Scale(Vector2Normalize(Vector2Subtract(m_world, e->phy.pos)), force_magnitude);
                        apply_force(&e->phy, force);
                    }
                    
                    if (follow_mouse) {
                        e->target = m_world;
                    }

                    // Collision with surfaces
                    for (int si = 0; si < surfaces.count; si++) {
                        Surface *surf = &surfaces.items[si];
                        surface_resolve_with_physics_object(surf, &e->phy, dt);
                    }

                    update_entity(e, dt);
                    
                    // e->pos = warp_in_bounds(e->pos, bounds);
                }

                // Update spider
                spider.phy.affected_by_gravity = true;
                spider.l_foot.affected_by_gravity = true;
                spider.r_foot.affected_by_gravity = true;

                // Collision with surfaces
                for (int si = 0; si < surfaces.count; si++) {
                    Surface *surf = &surfaces.items[si];
                    surface_resolve_with_physics_object(surf, &spider.phy, dt);
                    surface_resolve_with_physics_object(surf, &spider.l_foot, dt);
                    surface_resolve_with_physics_object(surf, &spider.r_foot, dt);
                }
                
                update_spider(&spider, dt);

                // Update radiuses
                for (int i = radiuses.count-1; i >= 0; --i) {
                    Radius *r = &radiuses.items[i];
                    r->pos = m_world;
                    if (r->r <= 0.f) {
                        darr_delete(radiuses, Radius, i);
                    } else {
                        r->r -= dt * force_magnitude * 100.f;
                    }

                }
                accumulator -= FIXED_STEP;
            }
        } break;
        case MODE_EDIT: {
        } break;
        case MODE_COUNT:
        default:
            ASSERT(false, "UNREACHABLE!");
        }

        // Draw
        BeginMode2D(cam);

        
        /// @DEBUG
        DrawCircleV(start, 2.f, WHITE);
        DrawCircleV(t_pos, 2.f, YELLOW);
        DrawCircleV(target, 2.f, WHITE);
        ///
       
        // Draw Surfaces
        for (size_t i = 0; i < surfaces.count; ++i) {
            Surface *surf = &surfaces.items[i];
            draw_surface(surf, debug_draw);
        }
        
        // Draw entities
        for (size_t i = 0; i < entities.count; ++i) {
            Entity *e = &entities.items[i];
            draw_entity(e, debug_draw);
        }

        // Draw radiuses
        for (int i = radiuses.count-1; i >= 0; --i) {
            Radius *r = &radiuses.items[i];
            Color c = WHITE;
            c.a = mapf(r->r, r->spawn_r, 0.f, 0, 255);
            DrawCircleLinesV(r->pos, r->r, c);
        }

        // Draw Spider
        draw_spider(&spider, debug_draw);

        EndMode2D();

        int y = 0;
        DRAW_INFO(DEFAULT_FONT_SIZE, GOLD, "mode: %s", mode_as_str(current_mode));
        if (debug_draw) {
            DRAW_INFO(DEFAULT_FONT_SIZE, WHITE, "cam: %f, %f (x%f)", cam.target.x, cam.target.y, cam.zoom);
            DRAW_INFO(DEFAULT_FONT_SIZE, WHITE, "m_world: %f, %f", m_world.x, m_world.y);
            DRAW_INFO(DEFAULT_FONT_SIZE, WHITE, "spider: %f, %f {%fkg, %fkg}", spider.phy.pos.x, spider.phy.pos.y, spider.l_foot.mass, spider.r_foot.mass);
            DRAW_INFO(DEFAULT_FONT_SIZE, WHITE, "Apply force: %s (%f)", do_apply_force ? "true" : "false", force_magnitude);
            DRAW_INFO(DEFAULT_FONT_SIZE, WHITE, "mass: %f", mass);
            DRAW_INFO(DEFAULT_FONT_SIZE, WHITE, "elasticity: %f", elasticity);

            DRAW_INFO(DEFAULT_FONT_SIZE, WHITE, "%s", "--------------------------------------------------");

            DRAW_INFO(DEFAULT_FONT_SIZE, RED, "Entities count: %zu", entities.count);
            DRAW_INFO(DEFAULT_FONT_SIZE, RED, "Radiuses count: %zu", radiuses.count);

            DRAW_INFO(DEFAULT_FONT_SIZE, WHITE, "%s", "--------------------------------------------------");
            DRAW_INFO(DEFAULT_FONT_SIZE, WHITE, "Spider LF on GROUND: %s", (spider.l_foot.collided_this_frame ? "true" : "false"));
            DRAW_INFO(DEFAULT_FONT_SIZE, WHITE, "Spider RF on GROUND: %s", (spider.r_foot.collided_this_frame ? "true" : "false"));

            BeginMode2D(cam);
            // DrawRectangleLinesEx(bounds, 1.f, WHITE);

            EndMode2D();
        }

        // Mode-specific Draw
        switch (current_mode) {
            case MODE_NORMAL: {
            } break;
            case MODE_EDIT: {
                if (debug_draw) {
                    DRAW_INFO(DEFAULT_FONT_SIZE, GOLD, "Kind: %s", entity_kind_as_str(edit_entity_kind));
                }
            } break;
            case MODE_COUNT:
            default:
                ASSERT(false, "UNREACHABLE!");
        }

        EndTextureMode();
        draw_ren_tex(ren_tex, SCREEN_WIDTH, SCREEN_HEIGHT);
        EndDrawing();
    }

    // cleanup();
    return 0;
}
