#ifndef SCREEN_H
#define SCREEN_H


#include <stdarg.h>
#include <stdio.h>


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


Screen * screen_alloc(FILE *, ScreenSize const);
void screen_destroy(Screen *);


void screen_render(Screen *);
void screen_render_at(Screen *, ScreenCoordinates const);
void screen_clean(Screen *);
void screen_printf(Screen *, ScreenCoordinates const, char const *, ...);
void screen_vprintf(Screen *, ScreenCoordinates const, char const *, va_list);


#endif
