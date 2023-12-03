#include "./screen.h"
#include "./base.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>


#define SCREEN_MAX_COLS 256
#define SCREEN_MAX_ROWS 128
#define SCREEN_MAX_LINE_SIZE 1024

#define MAX(a, b) ((a) < (b) ? (b) : (a))
#define MIN(a, b) ((a) > (b) ? (b) : (a))

#define BOOL int
#define FALSE 0
#define TRUE 1

#define VAL_INDEX(size, i, j) ((i) * (size)->cols + (j))


struct Screen {
    FILE * out;
    ScreenSize size;
    wchar_t * next_values;
    wchar_t * last_values;
    BOOL has_change;
    BOOL has_rendered;
};


static void screen_rerender(Screen *);
static void screen_render_new(Screen *);
static void screen_update_last_values(Screen *);
static void screen_initialize(Screen *);
static wchar_t * screen_get_next_row_at(Screen *, size_t);
static wchar_t * screen_get_last_row_at(Screen *, size_t);
static size_t buffer_get_len(ScreenSize const *);
static void buffer_clear(wchar_t *, ScreenSize const *);
static wchar_t * buffer_get_line(wchar_t *, ScreenSize const *, size_t);
static void remove_last_line(void);
static void remove_current_line(void);


Screen * screen_alloc(FILE * out, ScreenSize const * size) {
    Screen * screen = XRE_ALLOC(Screen, 1);
    XRE_ASSERT_ALLOC(screen);

    screen->out = out;
    screen->size = *size;

    size_t wc_size = sizeof(wchar_t);
    screen->next_values = XRE_ALLOC(wchar_t, buffer_get_len(size) * wc_size);
    XRE_ASSERT_ALLOC(screen->next_values);

    screen->last_values = XRE_ALLOC(wchar_t, buffer_get_len(size) * wc_size);
    XRE_ASSERT_ALLOC(screen->last_values);

    screen->has_rendered = FALSE;

    screen_initialize(screen);

    return screen;
}

void screen_destroy(Screen * screen) {
    RETURN_IF_NULL(screen);

    XRE_FREE(screen->next_values);
    XRE_FREE(screen->last_values);
    XRE_FREE(screen);
}


void screen_render(Screen * screen) {
    RETURN_IF_NULL(screen);

    if (!screen->has_change) {
        return;
    }

    if (screen->has_rendered) {
        screen_rerender(screen);
    } else {
        screen_render_new(screen);
    }

    screen->has_rendered = TRUE;
    screen_update_last_values(screen);
}


void screen_render_clear(Screen * screen) {
    RETURN_IF_NULL(screen);

    if (!screen->has_rendered) {
        return;
    }

    ScreenSize const * size = screen_get_size(screen);

    if (size->rows > 0) {
        remove_current_line();
    }
    for (size_t i = 0; i < size->rows - 1; i++) {
        remove_last_line();
    }

    screen->has_rendered = FALSE;
}


void screen_vwprintf(
    Screen * screen,
    ScreenCoordinates const * coords,
    wchar_t const * format,
    va_list args
) {
    RETURN_IF_NULL(screen);
    RETURN_IF_NULL(coords);

    ScreenSize const * screen_size = screen_get_size(screen);

    if (
        coords->y < 0
        || coords->x >= (int) screen_size->cols
        || coords->y >= (int) screen_size->rows
    ) {
        return;
    }

    size_t const max_len = screen_size->cols - coords->x;

    wchar_t buffer[max_len + 1];
    buffer[max_len] = L'\0';
    vswprintf(buffer, max_len + 1, format, args);

    size_t buffer_offset = MAX(-coords->x, 0);
    size_t const buffer_len = wcslen(buffer);

    if (buffer_offset >= buffer_len) {
        return;
    }

    size_t const print_len = buffer_len - buffer_offset;

    wchar_t * const screen_row = screen_get_next_row_at(screen, coords->y);
    wmemcpy(screen_row + MAX(coords->x, 0), buffer + buffer_offset, print_len);

    screen->has_change = TRUE;
}


void screen_vprintf(
    Screen * screen,
    ScreenCoordinates const * coords,
    char const * format,
    va_list args
) {
    RETURN_IF_NULL(screen);
    RETURN_IF_NULL(coords);

    ScreenSize const * screen_size = screen_get_size(screen);

    if (
        coords->y < 0
        || coords->x >= (int) screen_size->cols
        || coords->y >= (int) screen_size->rows
    ) {
        return;
    }

    size_t const max_len = screen_size->cols - MAX(coords->x, 0);

    char buffer[max_len + 1];
    buffer[max_len] = '\0';
    vsnprintf(buffer, max_len, format, args);

    screen_wprintf(screen, coords, L"%s", buffer);
}


void screen_wprintf(
    Screen * screen,
    ScreenCoordinates const * coords,
    wchar_t const * format,
    ...
) {
    va_list args;
    va_start(args, format);
    screen_vwprintf(screen, coords, format, args);
    va_end(args);
}


void screen_printf(
    Screen * screen,
    ScreenCoordinates const * coords,
    char const * format,
    ...
) {
    va_list args;
    va_start(args, format);
    screen_vprintf(screen, coords, format, args);
    va_end(args);
}


inline ScreenSize const * screen_get_size(Screen const * screen) {
    return &screen->size;
};


//------------------------------------------------------------------------------
// Private
//------------------------------------------------------------------------------


inline void screen_initialize(Screen * screen) {
    buffer_clear(screen->next_values, &screen->size);
    buffer_clear(screen->last_values, &screen->size);
    screen->has_change = TRUE;
}


void screen_rerender(Screen * screen) {
    fwprintf(screen->out, L"\033[G\033[%zuA\033[G", screen->size.rows -1);
    for (size_t i = 0; i < screen->size.rows; i++) {
        wchar_t * const line = screen_get_next_row_at(screen, i);
        wchar_t * const last_line = screen_get_last_row_at(screen, i);

        if (wcsncmp(line, last_line, screen->size.cols) != 0) {
            fwprintf(screen->out, L"%.*ls", (int)screen->size.cols, line);
        }
        fwprintf(screen->out, L"\033[G\033[B");
    }

    fflush(screen->out);
}


void screen_render_new(Screen * screen) {
    for (size_t i = 0; i < screen->size.rows; ++i) {
        if (i > 0) {
            fwprintf(screen->out, L"\n\r");
        }
        wchar_t * const line = screen_get_next_row_at(screen, i);
        fwprintf(screen->out, L"%.*ls", (int)screen->size.cols, line);
    }
    fwprintf(screen->out, L"\033[G");
    fflush(screen->out);
}


inline void screen_update_last_values(Screen * screen) {
    wchar_t * swap_value = screen->last_values;
    screen->last_values = screen->next_values;
    screen->next_values = swap_value;

    buffer_clear(screen->next_values, &screen->size);
    screen->has_change = FALSE;
}


inline wchar_t * screen_get_next_row_at(Screen * screen, size_t row_index) {
    return buffer_get_line(screen->next_values, &screen->size, row_index);
}


inline wchar_t * screen_get_last_row_at(Screen * screen, size_t row_index) {
    return buffer_get_line(screen->last_values, &screen->size, row_index);
}


inline size_t buffer_get_len(ScreenSize const * size) {
    return size->rows * size->cols;
}


inline void buffer_clear(wchar_t * buffer, ScreenSize const * size) {
    size_t const size_ = buffer_get_len(size);
    wmemset(buffer, L' ', size_);
}


inline wchar_t * buffer_get_line(
    wchar_t * buffer,
    ScreenSize const * size,
    size_t row_index
) {
    return buffer + row_index * size->cols;
}


inline void remove_last_line(void) {
    wprintf(L"\033[A\033[K");
};


inline void remove_current_line(void) {
    wprintf(L"\033[K");
};
