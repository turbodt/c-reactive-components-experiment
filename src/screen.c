#include "./screen.h"
#include "./base.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define SCREEN_MAX_COLS 256
#define SCREEN_MAX_ROWS 128
#define SCREEN_MAX_LINE_SIZE 1024

#define MAX(a, b) ((a) < (b) ? (b) : (a))
#define MIN(a, b) ((a) > (b) ? (b) : (a))

#define BOOL char
#define FALSE 0
#define TRUE 1

#define VAL_INDEX(size, i, j) ((i) * (size)->cols + (j))

struct Screen {
    FILE * out;
    ScreenSize size;
    char * next_values;
    char * last_values;
    BOOL has_render;
};


static void screen_init(Screen *, FILE *, ScreenSize const *);
static void screen_clean_up(Screen *);
static void screen_clear_next_values(Screen *);
static void screen_clear_last_values(Screen *);
static void screen_render_line(Screen *, size_t);
static void screen_render_header(Screen *);
static void screen_render_footer(Screen *);
static void remove_header(void);
static void remove_footer(void);
static void remove_last_line(void);
static void remove_current_line(void);


Screen * screen_alloc(FILE * out, ScreenSize const * size) {
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


void screen_render(Screen *screen) {
    ScreenSize const * size = screen_get_size(screen);

    if (strcmp(screen->last_values, screen->next_values) == 0) {
        return;
    }

    screen_render_clear(screen);

    screen_render_header(screen);
    for (size_t i = 0; i < size->rows; i++) {
        screen_render_line(screen, i);
    }
    screen_render_footer(screen);

    screen->has_render = TRUE;

    screen_clear_last_values(screen);

    char * swap_value = screen->next_values;
    screen->next_values = screen->last_values;
    screen->last_values = swap_value;
};


void screen_render_clear(Screen * screen) {
    if (!screen->has_render) {
        return;
    }

    ScreenSize const * size = screen_get_size(screen);

    remove_header();
    for (size_t i = 0; i < size->rows; i++) {
        remove_last_line();
    }
    remove_header();

    screen->has_render = FALSE;
};


void screen_vprintf(
    Screen * screen,
    ScreenCoordinates const * coords,
    char const * format,
    va_list props
) {
    ScreenCoordinates final_coords;
    ScreenSize const * size = screen_get_size(screen);

    if (coords->y < 0 || coords->y >= (int) size->rows) {
        return;
    }

    char buffer[size->rows];
    vsnprintf(buffer, size->cols, format, props);
    size_t str_len = strlen(buffer);

    if (coords->x + (int) str_len < 0 || coords->x >= (int) size->cols) {
        return;
    }

    size_t str_final_len = MIN(str_len, str_len + coords->x);
    if (str_final_len <= 0) {
        return;
    }

    final_coords.x = MAX((int) 0, coords->x);
    final_coords.y = coords->y;

    size_t str_offset = str_len - str_final_len;

    strncpy(
        screen->next_values + VAL_INDEX(size, final_coords.y, final_coords.x),
        buffer + str_offset,
        MIN((int) str_final_len, (int) size->cols - final_coords.x)
    );
};


void screen_printf(
    Screen * screen,
    ScreenCoordinates const * coords,
    char const * format,
    ...
) {
    va_list props;
    va_start(props, format);
    screen_vprintf(screen, coords, format, props);
    va_end(props);
};


inline ScreenSize const * screen_get_size(Screen const *screen) {
    return &screen->size;
};


//------------------------------------------------------------------------------
// Private
//------------------------------------------------------------------------------


void screen_init(Screen * screen, FILE * out, ScreenSize const * size) {
    screen->out = out;
    screen->has_render = FALSE;

    screen->size.rows = MIN(SCREEN_MAX_ROWS, size->rows);
    screen->size.cols = MIN(SCREEN_MAX_COLS, size->cols);
    size_t len = screen->size.rows * screen->size.cols;

    screen->next_values = XRE_ALLOC(char, len + 1);
    XRE_ASSERT_ALLOC(screen->next_values);

    screen->last_values = XRE_ALLOC(char, len + 1);
    XRE_ASSERT_ALLOC(screen->next_values);

    screen_clear_next_values(screen);
    screen_clear_last_values(screen);
};


inline void screen_clean_up(Screen *screen) {
    screen_clear_next_values(screen);

    XRE_FREE(screen->next_values);
    XRE_FREE(screen->last_values);
};


inline void screen_render_line(Screen * screen, size_t row_index) {
    ScreenSize const * size = screen_get_size(screen);
    fprintf(screen->out, "║");
    fwrite(
        screen->next_values + VAL_INDEX(size, row_index, 0),
        sizeof(char),
        size->cols,
        screen->out
    );
    fprintf(screen->out, "║\n");
};


inline void screen_render_header(Screen * screen) {
    ScreenSize const * size = screen_get_size(screen);

    fprintf(screen->out, "\n╔");
    for (size_t i = 0; i < size->cols; i++) {
        fprintf(screen->out, "═");
    }
    fprintf(screen->out, "╗\n");
};
inline void remove_header(void) {
    remove_last_line();
};


inline void screen_render_footer(Screen * screen) {
    ScreenSize const * size = screen_get_size(screen);

    fprintf(screen->out, "╚");
    for (size_t i = 0; i < size->cols; i++) {
        fprintf(screen->out, "═");
    }
    fprintf(screen->out, "╝");
    fprintf(screen->out, "\033[G");
};
inline void remove_footer(void) {
    remove_current_line();
};


void screen_clear_next_values(Screen *screen) {
    ScreenSize const * size = screen_get_size(screen);
    size_t len = VAL_INDEX(size, size->rows, 0);

    memset(screen->next_values, ' ', len);
    *(screen->next_values + len) = '\0';
};


void screen_clear_last_values(Screen *screen) {
    ScreenSize const * size = screen_get_size(screen);
    size_t len = VAL_INDEX(size, size->rows, 0);

    memset(screen->last_values, ' ', len);
    *(screen->last_values + len) = '\0';
};


inline void remove_last_line(void) {
    printf("\033[A\033[K");
};


inline void remove_current_line(void) {
    printf("\033[K");
};
