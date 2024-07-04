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
#define KEY_PRESS_DEBOUNCE_S 0.05


typedef struct Box {
    int x;
    int y;
    size_t width;
    size_t height;
} Box;


typedef struct CheckboxProps {
    BOOL const is_focused;
    char const * label;
    ScreenCoordinates position;
    XREStateInt * value_state;
} CheckboxProps;
typedef struct SelectorProps {
    int focused_option_index;
    int option_count;
    char const ** labels;
    int const * values;
    ScreenCoordinates position;
    XREStateInt * value_state;
} SelectorProps;


static Screen * screen = NULL;
static void screen_init(ScreenSize const * screen_size) {
    setlocale(LC_ALL, "");
    screen = screen_alloc(stdout, screen_size);
}
static void screen_clean_up(void) {
    screen_destroy(screen);
    screen = NULL;
}


static void app_component(XREContext *, va_list);
static void checkbox(XREContext *, va_list);
static void selector(XREContext *, va_list);


int main(void) {
    double const SPF = 0.016;
    TerminalSize terminal_size = get_terminal_size();
    ScreenSize const screen_size = {
        .rows=12,
        .cols=terminal_size.cols
    };

    kb_init();
    screen_init(&screen_size);
    XREContext * root_context = xre_context_root_alloc();

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


void app_component(XREContext * ctx, va_list props) {
    static const int color_option_count = 3;
    static int const cb_count = 2 + color_option_count;
    static char const * color_labels[] = {
        "No color",
        "Red text",
        "Green text"
    };
    static int const color_values[] = {0, 1, 2};

    int * should_exit = va_arg(props, int *);

    ScreenSize const * screen_size = screen_get_size(screen);
    XREStateInt * key_press_state = xre_use_int(ctx, EOF);
    XREStateInt * cb_index_state = xre_use_int(ctx, 0);
    XREStateInt * is_bold_state = xre_use_int(ctx, 0);
    XREStateInt * color_state = xre_use_int(ctx, 0);

    int cb_index = xre_state_get_int(cb_index_state);

    ScreenCoordinates text_coords = {0, 0};
    int pressed_key = kbhit();
    xre_state_set_int(key_press_state, pressed_key);
    int key_press = xre_state_get_int(key_press_state);
    ungetc(key_press, stdin);

    if (pressed_key == 27) {
        *should_exit = 1;
    } else if (pressed_key == 'j') {
        cb_index = (cb_index + 1)%cb_count;
        xre_state_set_int(cb_index_state, cb_index);
    } else if (pressed_key == 'k') {
        cb_index = (cb_index + cb_count - 1)%cb_count;
        xre_state_set_int(cb_index_state, cb_index);
    }

    int is_bold = xre_state_get_int(is_bold_state);
    int color = xre_state_get_int(color_state);

    char modifiers[32] = "";

    if (is_bold) {
        strcat(modifiers, "\033[30;0;1m");
    }

    switch (color) {
        case 1:
            strcat(modifiers, "\033[31m");
            break;
        case 2:
            strcat(modifiers, "\033[32m");
            break;
        default:
            strcat(modifiers, "\033[37m");
            break;
    }

    text_coords.y = 0;
    screen_printf(
        screen,
        &text_coords,
        "%sPress 'j', 'k' to go down/up and <space> to select/unselect\033[0m",
        modifiers
    );

    text_coords.y = screen_size->rows -1;
    screen_printf(
        screen,
        &text_coords,
        "%sPress <ESC> to terminate.\033[0m",
        modifiers
    );

    xre_use(
        ctx,
        "cb_bold",
        checkbox,
        (CheckboxProps) {
            .is_focused = cb_index == 0,
            .label = "Bold text",
            .position = (ScreenCoordinates){0, 2},
            .value_state=is_bold_state
        }
    );
    xre_use(
        ctx,
        "sel_color",
        selector,
        (SelectorProps) {
            .focused_option_index = cb_index - 1,
            .option_count = color_option_count,
            .labels = color_labels,
            .values = color_values,
            .position = (ScreenCoordinates){0, 3},
            .value_state=color_state
        }
    );
    xre_use(
        ctx,
        "cb_null",
        checkbox,
        (CheckboxProps) {
            .is_focused = cb_index == 1 + color_option_count,
            .label = "This checkbox does nothing",
            .position = (ScreenCoordinates){0, 3 + color_option_count},
            .value_state=NULL
        }
    );

    kbhit();
};


void checkbox(XREContext *ctx, va_list args) {
    static char const box_false_str[] = "   [ ]";
    static char const box_true_str[] = "   [X]";
    static char const box_foc_false_str[] = "-> [ ]";
    static char const box_foc_true_str[] = "-> [X]";

    CheckboxProps props = va_arg(args, CheckboxProps);
    XREStateInt * inner_value_state = xre_use_int(ctx, 0);

    XREStateInt * value_state;
    if (props.value_state == NULL) {
        value_state = inner_value_state;
    } else {
        value_state = props.value_state;
    }

    int value = xre_state_get_int(value_state);
    int pressed_key = kbhit();
    ungetc(pressed_key, stdin);

    if (props.is_focused && pressed_key == ' ') {
        value = (value + 1)%2;
        xre_state_set_int(value_state, value);
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


void selector(XREContext *ctx, va_list args) {
    (void) ctx;
    static char const box_false_str[] = "   ( )";
    static char const box_true_str[] = "   (X)";
    static char const box_foc_false_str[] = "-> ( )";
    static char const box_foc_true_str[] = "-> (X)";

    SelectorProps props = va_arg(args, SelectorProps);
    ScreenCoordinates position = props.position;

    int value = xre_state_get_int(props.value_state);
    int pressed_key = kbhit();
    ungetc(pressed_key, stdin);

    int focused_option_index = -1;
    if (
        props.focused_option_index >= 0
        && props.focused_option_index < props.option_count
    ) {
        focused_option_index = props.focused_option_index;
    }

    if (focused_option_index >= 0 && pressed_key == ' ') {
        value = props.values[focused_option_index];
        xre_state_set_int(props.value_state, value);
    }

    for (int i = 0; i < props.option_count; i++) {
        BOOL is_focused = props.focused_option_index == i;

        char const * box_str = NULL;
        if (is_focused && value == props.values[i]) {
            box_str = box_foc_true_str;
        } else if (is_focused && value != props.values[i]) {
            box_str = box_foc_false_str;
        } else if (!is_focused && value == props.values[i]) {
            box_str = box_true_str;
        } else if (!is_focused && value != props.values[i]) {
            box_str = box_false_str;
        }

        screen_printf(screen, &position, "%s %s", box_str, props.labels[i]);
        position.y++;
    }
};
