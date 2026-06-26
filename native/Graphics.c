#include "../native/native.h"
#include <raylib.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

// ── Color parsing ────────────────────────────────────────────────────────────

static Color parse_color(const char *name) {
    if (!name) return WHITE;

    if (name[0] == '#' && strlen(name) == 7) {
        unsigned int r, g, b;
        sscanf(name + 1, "%02x%02x%02x", &r, &g, &b);
        return (Color){ r, g, b, 255 };
    }

    if (strcmp(name, "black")   == 0) return BLACK;
    if (strcmp(name, "white")   == 0) return WHITE;
    if (strcmp(name, "red")     == 0) return RED;
    if (strcmp(name, "green")   == 0) return GREEN;
    if (strcmp(name, "blue")    == 0) return BLUE;
    if (strcmp(name, "yellow")  == 0) return YELLOW;
    if (strcmp(name, "orange")  == 0) return ORANGE;
    if (strcmp(name, "purple")  == 0) return PURPLE;
    if (strcmp(name, "pink")    == 0) return PINK;
    if (strcmp(name, "gray")    == 0) return GRAY;
    if (strcmp(name, "darkgray")== 0) return DARKGRAY;
    if (strcmp(name, "lightgray")== 0)return LIGHTGRAY;
    if (strcmp(name, "skyblue") == 0) return SKYBLUE;
    if (strcmp(name, "darkblue")== 0) return DARKBLUE;
    if (strcmp(name, "darkgreen")== 0)return DARKGREEN;
    if (strcmp(name, "maroon")  == 0) return MAROON;
    if (strcmp(name, "gold")    == 0) return GOLD;
    if (strcmp(name, "lime")    == 0) return LIME;
    if (strcmp(name, "brown")   == 0) return BROWN;
    if (strcmp(name, "beige")   == 0) return BEIGE;
    if (strcmp(name, "magenta") == 0) return MAGENTA;
    if (strcmp(name, "violet")  == 0) return VIOLET;

    return WHITE;
}

// ── Key parsing ──────────────────────────────────────────────────────────────

static int parse_key(const char *name) {
    if (!name) return KEY_NULL;

    if (strcmp(name, "right")  == 0) return KEY_RIGHT;
    if (strcmp(name, "left")   == 0) return KEY_LEFT;
    if (strcmp(name, "up")     == 0) return KEY_UP;
    if (strcmp(name, "down")   == 0) return KEY_DOWN;
    if (strcmp(name, "space")  == 0) return KEY_SPACE;
    if (strcmp(name, "enter")  == 0) return KEY_ENTER;
    if (strcmp(name, "escape") == 0) return KEY_ESCAPE;
    if (strcmp(name, "shift")  == 0) return KEY_LEFT_SHIFT;
    if (strcmp(name, "ctrl")   == 0) return KEY_LEFT_CONTROL;
    if (strcmp(name, "tab")    == 0) return KEY_TAB;
    if (strcmp(name, "back")   == 0) return KEY_BACKSPACE;

    // single letter keys
    if (strlen(name) == 1 && isalpha(name[0])) {
        return KEY_A + (toupper(name[0]) - 'A');
    }

    // digit keys
    if (strlen(name) == 1 && isdigit(name[0])) {
        return KEY_ZERO + (name[0] - '0');
    }

    return KEY_NULL;
}

// ── Helpers ──────────────────────────────────────────────────────────────────

static int arg_int(size_t argc, const char **argv, size_t i) {
    if (i >= argc || !argv[i]) return 0;
    return atoi(argv[i]);
}

static float arg_float(size_t argc, const char **argv, size_t i) {
    if (i >= argc || !argv[i]) return 0.0f;
    return (float)atof(argv[i]);
}

static const char *arg_str(size_t argc, const char **argv, size_t i) {
    if (i >= argc || !argv[i]) return "";
    return argv[i];
}

// ── Window ───────────────────────────────────────────────────────────────────

NativeValue Graphics_window(size_t argc, const char **argv) {
    int         w     = arg_int(argc, argv, 0);
    int         h     = arg_int(argc, argv, 1);
    const char *title = arg_str(argc, argv, 2);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(w, h, title);
    SetTargetFPS(60);
    return NATIVE_INT_VALUE(0);
}

NativeValue Graphics_fps(size_t argc, const char **argv) {
    SetTargetFPS(arg_int(argc, argv, 0));
    return NATIVE_INT_VALUE(0);
}

NativeValue Graphics_running(size_t argc, const char **argv) {
    (void)argc; (void)argv;
    if (WindowShouldClose()) return NATIVE_BOOL_VALUE(0);
    BeginDrawing();
    return NATIVE_BOOL_VALUE(1);
}

NativeValue Graphics_update(size_t argc, const char **argv) {
    (void)argc; (void)argv;
    EndDrawing();
    return NATIVE_INT_VALUE(0);
}

NativeValue Graphics_close(size_t argc, const char **argv) {
    (void)argc; (void)argv;
    CloseWindow();
    return NATIVE_INT_VALUE(0);
}

// ── Drawing ──────────────────────────────────────────────────────────────────

NativeValue Graphics_clear(size_t argc, const char **argv) {
    ClearBackground(parse_color(arg_str(argc, argv, 0)));
    return NATIVE_INT_VALUE(0);
}

NativeValue Graphics_rect(size_t argc, const char **argv) {
    int x = arg_int(argc, argv, 0);
    int y = arg_int(argc, argv, 1);
    int w = arg_int(argc, argv, 2);
    int h = arg_int(argc, argv, 3);
    Color c = parse_color(arg_str(argc, argv, 4));
    DrawRectangle(x, y, w, h, c);
    return NATIVE_INT_VALUE(0);
}

NativeValue Graphics_circle(size_t argc, const char **argv) {
    int   x = arg_int(argc, argv, 0);
    int   y = arg_int(argc, argv, 1);
    float r = arg_float(argc, argv, 2);
    Color c = parse_color(arg_str(argc, argv, 3));
    DrawCircle(x, y, r, c);
    return NATIVE_INT_VALUE(0);
}

NativeValue Graphics_line(size_t argc, const char **argv) {
    int   x1 = arg_int(argc, argv, 0);
    int   y1 = arg_int(argc, argv, 1);
    int   x2 = arg_int(argc, argv, 2);
    int   y2 = arg_int(argc, argv, 3);
    Color c  = parse_color(arg_str(argc, argv, 4));
    DrawLine(x1, y1, x2, y2, c);
    return NATIVE_INT_VALUE(0);
}

NativeValue Graphics_text(size_t argc, const char **argv) {
    const char *str  = arg_str(argc, argv, 0);
    int         x    = arg_int(argc, argv, 1);
    int         y    = arg_int(argc, argv, 2);
    int         size = arg_int(argc, argv, 3);
    Color       c    = parse_color(arg_str(argc, argv, 4));
    DrawText(str, x, y, size, c);
    return NATIVE_INT_VALUE(0);
}

NativeValue Graphics_pixel(size_t argc, const char **argv) {
    int   x = arg_int(argc, argv, 0);
    int   y = arg_int(argc, argv, 1);
    Color c = parse_color(arg_str(argc, argv, 2));
    DrawPixel(x, y, c);
    return NATIVE_INT_VALUE(0);
}

NativeValue Graphics_rectLines(size_t argc, const char **argv) {
    int x = arg_int(argc, argv, 0);
    int y = arg_int(argc, argv, 1);
    int w = arg_int(argc, argv, 2);
    int h = arg_int(argc, argv, 3);
    Color c = parse_color(arg_str(argc, argv, 4));
    DrawRectangleLines(x, y, w, h, c);
    return NATIVE_INT_VALUE(0);
}

// ── Input ────────────────────────────────────────────────────────────────────

NativeValue Graphics_keyDown(size_t argc, const char **argv) {
    return NATIVE_BOOL_VALUE(IsKeyDown(parse_key(arg_str(argc, argv, 0))));
}

NativeValue Graphics_keyPressed(size_t argc, const char **argv) {
    return NATIVE_BOOL_VALUE(IsKeyPressed(parse_key(arg_str(argc, argv, 0))));
}

NativeValue Graphics_mouseX(size_t argc, const char **argv) {
    (void)argc; (void)argv;
    return NATIVE_INT_VALUE(GetMouseX());
}

NativeValue Graphics_mouseY(size_t argc, const char **argv) {
    (void)argc; (void)argv;
    return NATIVE_INT_VALUE(GetMouseY());
}

NativeValue Graphics_mouseDown(size_t argc, const char **argv) {
    (void)argc; (void)argv;
    return NATIVE_BOOL_VALUE(IsMouseButtonDown(MOUSE_BUTTON_LEFT));
}

NativeValue Graphics_mousePressed(size_t argc, const char **argv) {
    (void)argc; (void)argv;
    return NATIVE_BOOL_VALUE(IsMouseButtonPressed(MOUSE_BUTTON_LEFT));
}

// ── Timing ───────────────────────────────────────────────────────────────────

NativeValue Graphics_dt(size_t argc, const char **argv) {
    (void)argc; (void)argv;
    return NATIVE_FLOAT_VALUE(GetFrameTime());
}

NativeValue Graphics_time(size_t argc, const char **argv) {
    (void)argc; (void)argv;
    return NATIVE_FLOAT_VALUE((float)GetTime());
}

NativeValue Graphics_width(size_t argc, const char **argv) {
    (void)argc; (void)argv;
    return NATIVE_INT_VALUE(GetScreenWidth());
}

NativeValue Graphics_height(size_t argc, const char **argv) {
    (void)argc; (void)argv;
    return NATIVE_INT_VALUE(GetScreenHeight());
}
