#include <config.h>
#include <common.h>
#include <entities.h>
#include <segment.h>
#include <leg.h>
#include <spider.h>
#include <physics_object.h>

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

bool coll_resolve_bounds(Rectangle bounds, Vector2 *pos, float radius) {
    Vector2 tl = v2(bounds.x, bounds.y);
    Vector2 tr = v2(bounds.x + bounds.width, bounds.y);
    Vector2 br = v2(bounds.x + bounds.width, bounds.y + bounds.height);
    Vector2 bl = v2(bounds.x, bounds.y + bounds.height);
    bool col = false;
    col |= coll_resolve_circle_line_segment(tl, tr, pos, radius);
    col |= coll_resolve_circle_line_segment(br, bl, pos, radius);
    col |= coll_resolve_circle_line_segment(tl, bl, pos, radius);
    col |= coll_resolve_circle_line_segment(tr, br, pos, radius);

    return col;
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

    // Edit
    Entity_kind edit_entity_kind = EK_BASE;
    
    /// @DEBUG
    bool do_apply_force = false;
    bool follow_mouse = false;
    float mass = 1.f;
    float force_magnitude = 1.f;
    /// 
    
    /// @DEBUG
    Leg l = make_leg(v2xx(0), 30.f);
    ///
    Rectangle bounds = {
        .x = -width*1.f,
        .y = -height*1.f,
        .width = width*2.f,
        .height = height*2.f,
    };

    while (!quit && !WindowShouldClose()) {
        arena_reset(&temp_arena);
        const char* title_str = arena_alloc_str(temp_arena, "%s | %d FPS", window_name, GetFPS());

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
            cam.target.x -= GetFrameTime() * cam_speed * (IsKeyDown(KEY_LEFT_SHIFT) ? 2.f : 1.f);
        }
        if (IsKeyDown(KEY_D)) {
            cam.target.x += GetFrameTime() * cam_speed * (IsKeyDown(KEY_LEFT_SHIFT) ? 2.f : 1.f);
        }
        if (IsKeyDown(KEY_W)) {
            cam.target.y -= GetFrameTime() * cam_speed * (IsKeyDown(KEY_LEFT_SHIFT) ? 2.f : 1.f);
        }
        if (IsKeyDown(KEY_S)) {
            cam.target.y += GetFrameTime() * cam_speed * (IsKeyDown(KEY_LEFT_SHIFT) ? 2.f : 1.f);
        }

        if (IsKeyDown(KEY_E)) {
            cam.zoom += GetFrameTime();
        }

        if (IsKeyDown(KEY_Q)) {
            cam.zoom -= GetFrameTime();
            if (cam.zoom <= 0.001f) cam.zoom = 0.001f;
        }
        
        if (IsKeyPressed(KEY_ZERO)) cam.zoom = 1.f;


        // @DEBUG
        if (IsKeyDown(KEY_ONE)) {
            mass -= GetFrameTime();
            if (mass <= 1.f) mass = 1.f;
        }

        if (IsKeyDown(KEY_TWO)) {
            mass += GetFrameTime();
        }

        if (IsKeyPressed(KEY_SPACE)) {
            Entity e = make_entity(m_world, edit_entity_kind);
            e.phy.friction = 0.001f;
            e.phy.mass = mass;
            darr_append(entities, e);
        }
        do_apply_force   = IsKeyDown(KEY_X);
        follow_mouse  = IsKeyDown(KEY_C);
        
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
        l.start = m_world;
        update_leg_end_to_start(&l);
        l.end = v2xx(0);
        update_leg_start_to_end(&l);
        ///
        
        // Mode-specific Update
        switch (current_mode) {
        case MODE_NORMAL: {
            // Update entities 
            for (size_t i = 0; i < entities.count; ++i) {
                Entity *e = &entities.items[i];
                if (e->dead) continue;

                if (do_apply_force) {
                    Vector2 force = Vector2Scale(Vector2Normalize(Vector2Subtract(m_world, e->phy.pos)), force_magnitude);
                    apply_force(&e->phy, force);
                }
                
                if (follow_mouse) {
                    e->target = m_world;
                }

                if (coll_resolve_bounds(bounds, &e->phy.pos, entity_radius(e))) {
                    e->phy.affected_by_gravity = false;
                    e->phy.vel = Vector2Scale(e->phy.vel, -0.25f);
                }

                update_entity(e);
                
                // e->pos = warp_in_bounds(e->pos, bounds);
            }

            // Update spider
            spider.phy.affected_by_gravity = true;
            spider.l_foot.affected_by_gravity = true;
            spider.r_foot.affected_by_gravity = true;

            if (coll_resolve_bounds(bounds, &spider.phy.pos, entity_radius((Entity *)&spider))) {
                spider.phy.affected_by_gravity = false;
                spider.phy.vel = Vector2Scale(spider.phy.vel, -0.25f);
            }

            if (coll_resolve_bounds(bounds, &spider.l_foot.pos, 2.f)) {
                spider.l_foot.affected_by_gravity = false;
                spider.l_foot.vel = Vector2Scale(spider.l_foot.vel, -0.25f);
            }

            if (coll_resolve_bounds(bounds, &spider.r_foot.pos, 2.f)) {
                spider.r_foot.affected_by_gravity = false;
                spider.r_foot.vel = Vector2Scale(spider.r_foot.vel, -0.25f);
            }

            update_spider(&spider);

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
        // draw_leg(&l, debug_draw);
        ///
       
        // Draw entities
        for (size_t i = 0; i < entities.count; ++i) {
            Entity *e = &entities.items[i];
            draw_entity(e, debug_draw);
        }

        // Draw Spider
        draw_spider(&spider, debug_draw);

        EndMode2D();

        int y = 0;
        DRAW_INFO(DEFAULT_FONT_SIZE, GOLD, "mode: %s", mode_as_str(current_mode));
        if (debug_draw) {
            DRAW_INFO(DEFAULT_FONT_SIZE, WHITE, "cam: %f, %f (x%f)", cam.target.x, cam.target.y, cam.zoom);
            DRAW_INFO(DEFAULT_FONT_SIZE, WHITE, "m_world: %f, %f", m_world.x, m_world.y);
            DRAW_INFO(DEFAULT_FONT_SIZE, WHITE, "spider: %f, %f", spider.phy.pos.x, spider.phy.pos.y);
            DRAW_INFO(DEFAULT_FONT_SIZE, WHITE, "Apply force: %s (%f)", do_apply_force ? "true" : "false", force_magnitude);
            DRAW_INFO(DEFAULT_FONT_SIZE, WHITE, "mass: %f", mass);
            DRAW_INFO(DEFAULT_FONT_SIZE, WHITE, "Entities count: %zu", entities.count);

            BeginMode2D(cam);
            DrawRectangleLinesEx(bounds, 1.f, WHITE);

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
