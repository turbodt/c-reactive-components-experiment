#include "./screen.h"
#include "./base.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define SCREEN_MAX_COLS 80
#define SCREEN_MAX_ROWS 96
#define SCREEN_MAX_LINE_SIZE 1024

#define MAX(a, b) ((a) < (b) ? (b) : (a))
#define MIN(a, b) ((a) > (b) ? (b) : (a))


struct Screen {
    FILE * out;
    ScreenSize size;
    char values[SCREEN_MAX_ROWS][SCREEN_MAX_COLS + 1];
    ScreenCoordinates rendered_at_offset;
};


static void screen_init(Screen *, FILE *, ScreenSize const);
static void screen_clean_up(Screen *);
static void screen_clear(Screen *);
static void screen_render_line(Screen *, size_t, int);
static void screen_render_header(Screen *);
static void screen_render_footer(Screen *);
static void remove_header(void);
static void remove_footer(void);
static void remove_last_line(void);


Screen * screen_alloc(FILE * out, ScreenSize const size) {
    Screen * screen = XRE_ALLOC(Screen, 1);
    XRE_ASSERT_ALLOC(screen);

    screen_init(screen, out, size);

    return screen;
};


void screen_destroy(Screen *screen) {
    RETURN_IF_NULL(screen);

    screen_clean_up(screen);
    XRE_FREE(screen);
};


void screen_render_at(Screen *screen, ScreenCoordinates const offset) {
    ScreenSize const size = screen->size;

    screen_render_header(screen);
    for (size_t i = 0; i < size.rows + offset.y && i < SCREEN_MAX_ROWS; i++) {
        screen_render_line(screen, i, offset.x);
        fprintf(screen->out, "\n");
    }
    screen_render_footer(screen);

    screen_clear(screen);
    screen->rendered_at_offset = offset;
};


inline void screen_render(Screen *screen) {
    screen_render_at(screen, (ScreenCoordinates){0,0});
}

void screen_clean(Screen * screen) {
    ScreenCoordinates const offset = screen->rendered_at_offset;
    ScreenSize const size = screen->size;
    remove_header();
    for (size_t i = 0; i < size.rows + offset.y && i < SCREEN_MAX_ROWS; i++) {
        remove_last_line();
    }
    remove_header();

    screen->rendered_at_offset = (ScreenCoordinates){0,0};
};


void screen_vprintf(
    Screen * screen,
    ScreenCoordinates const coords,
    char const * format,
    va_list props
) {
    ScreenSize const size = screen->size;
    if (coords.y < 0 || coords.y >= (int) size.rows) {
        return;
    }

    char buffer[SCREEN_MAX_COLS];
    vsnprintf(buffer, SCREEN_MAX_COLS, format, props);
    size_t str_len = strlen(buffer);

    if (coords.x + (int) str_len < 0 || coords.x >= (int) size.cols) {
        return;
    }

    size_t str_final_len = MIN(str_len, str_len + coords.x);
    if (str_final_len <= 0) {
        return;
    }

    ScreenCoordinates final_coords = {
        MAX((int) 0, coords.x),
        coords.y
    };

    size_t str_offset = str_len - str_final_len;

    strncpy(
        screen->values[final_coords.y] + final_coords.x,
        buffer + str_offset,
        MIN((int) str_final_len, (int) size.cols - final_coords.x)
    );
};


void screen_printf(
    Screen * screen,
    ScreenCoordinates const coords,
    char const * format,
    ...
) {
    va_list props;
    va_start(props, format);
    screen_vprintf(screen, coords, format, props);
    va_end(props);
};


//------------------------------------------------------------------------------
// Private
//------------------------------------------------------------------------------


void screen_init(Screen * screen, FILE * out, ScreenSize const size) {
    screen->rendered_at_offset = (ScreenCoordinates){0,0};
    screen->out = out;
    screen->size = size;
    screen_clear(screen);
};


inline void screen_clean_up(Screen *screen) {
    screen_clear(screen);
};


inline void screen_render_line(Screen * screen, size_t row_index, int x) {
    fprintf(screen->out, "║%s║", screen->values[row_index] + x);
};


inline void screen_render_header(Screen * screen) {
    fprintf(screen->out, "╔");
    for (size_t i = 0; i < SCREEN_MAX_COLS; i++) {
        fprintf(screen->out, "═");
    }
    fprintf(screen->out, "╗\n");
};
inline void remove_header(void) {
    remove_last_line();
};


inline void screen_render_footer(Screen * screen) {
    fprintf(screen->out, "╚");
    for (size_t i = 0; i < SCREEN_MAX_COLS; i++) {
        fprintf(screen->out, "═");
    }
    fprintf(screen->out, "╝\n");
};
inline void remove_footer(void) {
    remove_last_line();
};

void screen_clear(Screen *screen) {
    for (size_t i = 0; i < SCREEN_MAX_ROWS; i++) {
        for (size_t j = 0; j < SCREEN_MAX_COLS; j++) {
            screen->values[i][j] = ' ';
        }
        screen->values[i][SCREEN_MAX_COLS] = '\0';
    }
};


inline void remove_last_line(void) {
    printf("\033[A\033[K");
};
