#ifndef EXAMPLES_UTILS_SCREEN_H
#define EXAMPLES_UTILS_SCREEN_H


#include <stdarg.h>
#include <stdio.h>
#include <wchar.h>


struct Screen;
typedef struct Screen Screen;

struct ScreenCoordinates {
    int x;
    int y;
};
typedef struct ScreenCoordinates ScreenCoordinates;


struct ScreenSize {
    size_t rows;
    size_t cols;
};
typedef struct ScreenSize ScreenSize;


Screen * screen_alloc(FILE *, ScreenSize const *);
void screen_destroy(Screen *);


void screen_render(Screen *);
void screen_render_clear(Screen *);
void screen_printf(Screen *, ScreenCoordinates const *, char const *, ...);
void screen_wprintf(Screen *, ScreenCoordinates const *, wchar_t const *, ...);
void screen_vwprintf(
    Screen *,
    ScreenCoordinates const *,
    wchar_t const *,
    va_list
);
void screen_vprintf(
    Screen *,
    ScreenCoordinates const *,
    char const *,
    va_list
);
ScreenSize const * screen_get_size(Screen const *);

#endif
