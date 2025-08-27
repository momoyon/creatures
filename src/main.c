#define ENGINE_IMPLEMENTATION
#include <engine.h>

#define COMMONLIB_REMOVE_PREFIX
#define COMMONLIB_IMPLEMENTATION
#include <commonlib.h>

#define STB_DS_IMPLEMENTATION
#include <stb_ds.h>

#define log_info_a(console, fmt, ...) log_info_console((console), fmt, __VA_ARGS__); log_info(fmt, __VA_ARGS__)

#define FACTOR 105
#define SCREEN_WIDTH (16 * FACTOR)
#define SCREEN_HEIGHT (9 * FACTOR)
#define SCREEN_SCALE 0.5

typedef enum {
    MODE_NORMAL,
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
    case MODE_COUNT:
    default:
        ASSERT(false, "UNREACHABLE!");
    }
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
    Vector2 mpos_from = { 0 };

    bool quit = false;

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

        BeginTextureMode(ren_tex);
        ClearBackground(BLACK);

        // Update
        // Mode-specific Update
        switch (current_mode) {
        case MODE_NORMAL: {
        } break;
        case MODE_COUNT:
        default:
            ASSERT(false, "UNREACHABLE!");
        }

        // Draw
        BeginMode2D(cam);
        EndMode2D();

        if (debug_draw) {
        }

        // Mode-specific Draw
        switch (current_mode) {
        case MODE_NORMAL: {
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
