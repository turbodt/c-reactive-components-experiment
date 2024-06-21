#include <locale.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "xre.h"
#include "../utils/kbhit.h"
#include "../utils/terminal.h"
#include "../utils/msleep.h"
#include "../utils/screen.h"
#include "../hooks/use_time.h"
#include "../hooks/use_timer.h"


#define BOOL unsigned int


struct Box {
    int x;
    int y;
    size_t width;
    size_t height;
};


struct CheckboxProps {
    BOOL const is_focused;
    char const * label;
    ScreenCoordinates position;
    struct XREStateInt * value_state;
};


#define KEY_PRESS_DEBOUNCE_S 0.05


static Screen * screen = NULL;
static void screen_init(struct ScreenSize const * screen_size) {
    setlocale(LC_ALL, "");
    screen = screen_alloc(stdout, screen_size);
}
static void screen_clean_up(void) {
    screen_destroy(screen);
    screen = NULL;
}


static void app_component(struct XREContext *, va_list);
static void checkbox(struct XREContext *, va_list);


int main(void) {
    double const SPF = 0.016;
    struct TerminalSize terminal_size = get_terminal_size();
    struct ScreenSize const screen_size = {
        .rows=10,
        .cols=terminal_size.cols
    };

    kb_init();
    screen_init(&screen_size);
    struct XREContext * root_context = xre_context_root_alloc();

    int exit = 0;
    while (!exit) {
        xre_use_root(root_context, app_component, &exit);
        screen_render(screen);

        msleep((long int)(1000 * SPF));
    }
    screen_render_clear(screen);

    xre_context_destroy(root_context);
    screen_clean_up();
    kb_clean_up();
}


static struct XREEffectCleanUp * app_component_key_press_effect(va_list args) {
    struct XREStateInt * count_state = va_arg(args, struct XREStateInt *);
    int count = xre_state_get_int(count_state);
    count++;
    xre_state_set_int(count_state, count);

    return NULL;
}


void app_component(struct XREContext * ctx, va_list props) {

    int * should_exit = va_arg(props, int *);

    ScreenSize const * screen_size = screen_get_size(screen);
    struct XREStateInt * key_press_state = xre_use_int(ctx, EOF);
    struct XREStateInt * key_press_count_state = xre_use_int(ctx, 0);
    int key_press_count = xre_state_get_int(key_press_count_state);
    struct XREStateInt * key_change_count_state = xre_use_int(ctx, 0);

    struct XREStateInt * cb_index_state = xre_use_int(ctx, 0);
    int cb_index = xre_state_get_int(cb_index_state);

    ScreenCoordinates text_coords = {0, 0};
    int pressed_key = kbhit();
    xre_state_set_int(key_press_state, pressed_key);
    int key_press = xre_state_get_int(key_press_state);
    ungetc(key_press, stdin);

    if (pressed_key == 27) {
        *should_exit = 1;
    } else if (pressed_key == 'k') {
        cb_index = (cb_index + 1)%2;
        xre_state_set_int(cb_index_state, cb_index);
    } else if (pressed_key == 'j') {
        cb_index = (cb_index + 2 - 1)%2;
        xre_state_set_int(cb_index_state, cb_index);
    } else if (pressed_key != EOF) {
        key_press_count++;
        xre_state_set_int(key_press_count_state, key_press_count);
    } else {
    }

    struct XRERef const * const effect_deps[] = {&key_press_state->ref , NULL};
    xre_use_effect(
        ctx,
        app_component_key_press_effect,
        effect_deps,
        key_change_count_state
    );

    int key_change_count = xre_state_get_int(key_change_count_state);

    text_coords.y = 0;
    screen_printf(screen, &text_coords, "Key press count: %d", key_press_count);
    text_coords.y = 1;
    screen_printf(screen, &text_coords, "Key change count: %d", key_change_count);
    text_coords.y = 2;
    screen_printf(screen, &text_coords, "Key pressed code: %d", (unsigned char)key_press);

    struct XREStateInt * cb1_value_state = xre_use_int(ctx, 0);
    xre_use(
        "My example checkbox",
        ctx,
        checkbox,
        (struct CheckboxProps) {
            .is_focused = cb_index == 0,
            .label = "This is a checkbox",
            .position = (ScreenCoordinates){0, 4},
            .value_state=cb1_value_state
        }
    );
    struct XREStateInt * cb2_value_state = xre_use_int(ctx, 0);
    xre_use(
        "My example checkbox two",
        ctx,
        checkbox,
        (struct CheckboxProps) {
            .is_focused = cb_index == 1,
            .label = "This is another checkbox",
            .position = (ScreenCoordinates){0, 5},
            .value_state=cb2_value_state
        }
    );

    text_coords.y = screen_size->rows -1;
    screen_printf(screen, &text_coords, "Press <ESC> to terminate.");

    kbhit();
};


void checkbox(struct XREContext *ctx, va_list args) {
    (void) ctx;

    //static char const box_false_str[] = "☐";
    //static char const box_true_str[] = "☑";
    static char const box_false_str[] = "[ ]";
    static char const box_true_str[] = "[X]";
    static char const box_foc_false_str[] = "> <";
    static char const box_foc_true_str[] = ">X<";

    struct CheckboxProps props = va_arg(args, struct CheckboxProps);

    int value = xre_state_get_int(props.value_state);
    int pressed_key = kbhit();
    ungetc(pressed_key, stdin);

    if (props.is_focused && pressed_key == ' ') {
        value = (value + 1)%2;
        xre_state_set_int(props.value_state, value);
    }

    char const * box_str = NULL;
    if (props.is_focused && value) {
        box_str = box_foc_true_str;
    } else if (props.is_focused && !value) {
        box_str = box_foc_false_str;
    } else if (!props.is_focused && value) {
        box_str = box_true_str;
    } else if (!props.is_focused && !value) {
        box_str = box_false_str;
    }

    screen_printf(screen, &props.position, "%s %s", box_str, props.label);
}
