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
#include "../hooks/use_timespec.h"


struct Box {
    int x;
    int y;
    size_t width;
    size_t height;
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


//------------------------------------------------------------------------------


void draw_box(struct Box const *box, wchar_t bg, int has_border) {
    has_border = has_border ? 1 : 0;

    size_t line_len = box->width + 2 * has_border;

    wchar_t line[line_len + 1];
    line[line_len] = L'\0';
    ScreenCoordinates coords = {
        box->x -1 * has_border,
        box->y -1 * has_border
    };

    if (bg != L'\0') {
        wmemset(line, bg, line_len);
        if (has_border) {
            line[0] = L'║';
            line[line_len - 1] = L'║';
        }

        for (int i = 0; i < (int) box->height; i++) {
            coords.y = box->y + i;
            screen_wprintf(screen, &coords, line);
        }
    }

    if (!has_border) {
        return;
    }

    if (bg == L'\0'){
        wchar_t s[2] = L"║";
        for (int i = 0; i < (int) box->height; i++) {
            coords.y = box->y + i;
            coords.x = box->x - 1;
            screen_wprintf(screen, &coords, s);
            coords.x = box->x + box->width;
            screen_wprintf(screen, &coords, s);
        }
    }

    wmemset(line, L'═', line_len);

    line[0] = L'╔';
    line[line_len - 1] = L'╗';
    coords.x = box->x -1;
    coords.y = box->y - 1;
    screen_wprintf(screen, &coords, line);

    line[0] = L'╚';
    line[line_len - 1] = L'╝';
    coords.x = box->x -1;
    coords.y = box->y + box->height;
    screen_wprintf(screen, &coords, line);
};


//------------------------------------------------------------------------------


char use_time_interval(struct XREContext *ctx, double seconds) {
    struct StateTimeSpec * last_time_state = xre_use_timespec(
        ctx,
        (struct timespec){0,0}
    );
    struct timespec last_time = xre_state_get_timespec(last_time_state);

    struct timespec now = {0, 0};
    clock_gettime(CLOCK_MONOTONIC, &now);

    long int sdiff = now.tv_sec - last_time.tv_sec;
    long int ndiff = ((sdiff * 1000000000L) + now.tv_nsec) - last_time.tv_nsec;

    if ((long int)(seconds * 1000000000L) > ndiff) {
        return 0;
    };

    xre_state_set_timespec(last_time_state, now);
    return 1;
}


//------------------------------------------------------------------------------


int use_pressed_key(struct XREContext *ctx) {
    struct XREStateInt * last_pressed_state = xre_use_int(ctx, EOF);
    char debouce_time_has_passed = use_time_interval(ctx, KEY_PRESS_DEBOUNCE_S);

    int last_pressed = xre_state_get_int(last_pressed_state);
    int key_c = kbhit();
    ungetc(key_c, stdin);

    if (last_pressed != EOF && !debouce_time_has_passed) {
        return EOF;
    }

    xre_state_set_int(last_pressed_state, key_c);

    return xre_state_get_int(last_pressed_state);
}


//------------------------------------------------------------------------------


double use_fps(struct XREContext * ctx, double interval_s) {

    char has_timer_passed = use_time_interval(ctx, interval_s);
    struct XREStateInt * frame_count_state = xre_use_int(ctx, 0);
    struct XREStateDouble * fps_state = xre_use_double(ctx, 0.0);

    int frame_count = xre_state_get_int(frame_count_state);

    if (has_timer_passed) {
        xre_state_set_double(fps_state, (double) frame_count / interval_s);
        xre_state_set_int(frame_count_state, 0);
    } else {
        frame_count++;
        xre_state_set_int(frame_count_state, frame_count);
    }

    return xre_state_get_double(fps_state);
};


//------------------------------------------------------------------------------


void box_screen_component(struct XREContext * ctx, va_list props) {
    (void) props;

    static double const x_delta = 1.5;
    static double const y_delta = 1.0;

    int pressed_key = use_pressed_key(ctx);

    struct XREStateDouble * pos_x_state = xre_use_double(ctx, 37.5);
    double pos_x = xre_state_get_double(pos_x_state);
    struct XREStateDouble * pos_y_state = xre_use_double(ctx, 4.0);
    double pos_y = xre_state_get_double(pos_y_state);
    struct XREStateSize * size_width_state = xre_use_size(ctx, 4);
    size_t size_width = xre_state_get_size(size_width_state);
    struct XREStateSize * size_height_state = xre_use_size(ctx, 2);
    size_t size_height = xre_state_get_size(size_height_state);
    struct XREStateInt * pos_x_change_counter_state = xre_use_int(ctx, 0);
    int pos_x_change_counter = xre_state_get_int(pos_x_change_counter_state);

    struct Box box = {(int) pos_x, (int) pos_y, size_width, size_height};
    ScreenSize const * screen_size = screen_get_size(screen);

    ScreenCoordinates text_coords = {1, 0};

    if (pressed_key == 'k' && box.y > 0) {
        pos_y -= y_delta;
        xre_state_set_double(pos_y_state, pos_y);
    } else if (pressed_key == 'j' && box.y + box.height < screen_size->rows) {
        pos_y += y_delta;
        xre_state_set_double(pos_y_state, pos_y);
    } else if (pressed_key == 'h' && box.x > 0) {
        pos_x -= x_delta;
        xre_state_set_double(pos_x_state, pos_x);
    } else if (pressed_key == 'l' && box.x + box.width < screen_size->cols) {
        pos_x += x_delta;
        xre_state_set_double(pos_x_state, pos_x);
    } else if (pressed_key == 'K' && box.height > 1) {
        size_height--;
        xre_state_set_size(size_height_state, size_height);
    } else if (pressed_key == 'J' && box.y + box.height < screen_size->rows) {
        size_height++;
        xre_state_set_size(size_height_state, size_height);
    } else if (pressed_key == 'H' && box.width > 1) {
        size_width--;
        xre_state_set_size(size_width_state, size_width);
    } else if (pressed_key == 'L' && box.x + box.width < screen_size->cols) {
        size_width++;
        xre_state_set_size(size_width_state, size_width);
    }

    if (xre_state_double_has_changed(pos_x_state)) {
        pos_x_change_counter++;
        xre_state_set_int(pos_x_change_counter_state, pos_x_change_counter);
    }

    draw_box(&box, L'░', 0);

    text_coords.y = screen_size->rows -4;
    screen_printf(screen, &text_coords, "X-Pos has changed %d times.", pos_x_change_counter);
    text_coords.y = screen_size->rows -3;
    screen_printf(screen, &text_coords, "Use h, j, k, l to move.");
    text_coords.y = screen_size->rows -2;
    screen_printf(screen, &text_coords, "Use H, J, K, L to change size.");
};


//------------------------------------------------------------------------------


void title_screen_component(struct XREContext * ctx, va_list props) {
    (void) props;
    (void) ctx;

    static char const text[] =
        "This is a screen with some text in it\n"
        "\n"
        "         _______     ____  ____  ________  \n"
        "        |_   __ \\   |_  _||_  _||_   __  | \n"
        "          | |__) |    \\ \\  / /    | |_ \\_| \n"
        "          |  __ /      > `' <     |  _| _  \n"
        "         _| |  \\ \\_  _/ /'`\\ \\_  _| |__/ | \n"
        "        |____| |___||____||____||________| \n"
        "\n"
        "and a title.\n"
        "";

    ScreenCoordinates text_coords = {4, 1};

    for (char const * start_ptr = text; *start_ptr != '\0'; start_ptr++) {
        size_t line_len = 0;
        char const * end_ptr = start_ptr;

        while (*end_ptr != '\0' && *end_ptr != '\n') {
            end_ptr++;
            line_len++;
        };

        char * line_str = (char *) malloc(sizeof(char) * (line_len + 1));
        line_str[line_len] = '\0';

        strncpy(line_str, start_ptr, line_len);

        screen_printf(screen, &text_coords, "%s", line_str);

        free(line_str);

        start_ptr = end_ptr;

        text_coords.y++;
    }

};


//------------------------------------------------------------------------------


struct XREEffectCleanUp * color_selection_effect(va_list args) {
    static char const text_template[] = "RGB(%d, %d, %d)";

    struct XREStateInt * red_state = va_arg(args, struct XREStateInt *);
    struct XREStateInt * green_state = va_arg(args, struct XREStateInt *);
    struct XREStateInt * blue_state = va_arg(args, struct XREStateInt *);
    struct XREStateString * text_state = va_arg(args, struct XREStateString *);

    int red_value = xre_state_get_int(red_state);
    int green_value = xre_state_get_int(green_state);
    int blue_value = xre_state_get_int(blue_state);

    char text[20];
    snprintf(text, 19, text_template, red_value, green_value, blue_value);
    xre_state_set_string(text_state, text);
    return NULL;
};


void color_selector_screen_component(struct XREContext * ctx, va_list props) {
    (void) props;
    (void) ctx;

    ScreenSize const * screen_size = screen_get_size(screen);
    size_t const bar_height = screen_size->rows - 8;

    struct XREStateInt * red_state = xre_use_int(ctx, 0);
    int red_value = xre_state_get_int(red_state);
    struct XREStateInt * green_state = xre_use_int(ctx, 0);
    int green_value = xre_state_get_int(green_state);
    struct XREStateInt * blue_state = xre_use_int(ctx, 0);
    int blue_value = xre_state_get_int(blue_state);
    struct XREStateInt * selected_index_state = xre_use_int(ctx, 0);
    int selected_index = xre_state_get_int(selected_index_state);

    struct XREStateString * text_state = xre_use_string(ctx, "");

    int pressed_key = use_pressed_key(ctx);


    if (pressed_key == 'k' && selected_index == 0 && red_value < 255) {
        red_value++;
        xre_state_set_int(red_state, red_value);
    } else if (pressed_key == 'k' && selected_index == 1 && green_value < 255) {
        green_value++;
        xre_state_set_int(green_state, green_value);
    } else if (pressed_key == 'k' && selected_index == 2 && blue_value < 255) {
        blue_value++;
        xre_state_set_int(blue_state, blue_value);
    } else if (pressed_key == 'j' && selected_index == 0 && red_value > 0) {
        red_value--;
        xre_state_set_int(red_state, red_value);
    } else if (pressed_key == 'j' && selected_index == 1 && green_value > 0) {
        green_value--;
        xre_state_set_int(green_state, green_value);
    } else if (pressed_key == 'j' && selected_index == 2 && blue_value > 0) {
        blue_value--;
        xre_state_set_int(blue_state, blue_value);
    } else if (pressed_key == 'h' && selected_index > 0) {
        selected_index--;
        xre_state_set_int(selected_index_state, selected_index);
    } else if (pressed_key == 'l' && selected_index < 2) {
        selected_index++;
        xre_state_set_int(selected_index_state, selected_index);
    }


    struct XRERef const * const effect_deps[] = {
        &red_state->ref,
        &green_state->ref,
        &blue_state->ref,
        NULL
    };

    xre_use_effect(
        ctx,
        color_selection_effect,
        effect_deps,
        red_state,
        green_state,
        blue_state,
        text_state
    );

    struct Box box = {screen_size->cols / 4, 4, 1, bar_height};

    box.x = screen_size->cols / 4;
    box.height = bar_height;
    box.y = 4;
    draw_box(&box, selected_index == 0 ? L'+' : L' ', 1);
    box.height = (size_t)((float)(bar_height*red_value)/255);
    box.y = 4 + bar_height - box.height;
    draw_box(&box, L'█', 0);

    box.x = screen_size->cols / 2;
    box.height = bar_height;
    box.y = 4;
    draw_box(&box, selected_index == 1 ? L'+' : L' ', 1);
    box.height = (size_t)((float)(bar_height*green_value)/255);
    box.y = 4 + bar_height - box.height;
    draw_box(&box, L'█', 0);

    box.x = 3*screen_size->cols / 4;
    box.height = bar_height;
    box.y = 4;
    draw_box(&box, selected_index == 2 ? L'+' : L' ', 1);
    box.height = (size_t)((float)(bar_height*blue_value)/255);
    box.y = 4 + bar_height - box.height;
    draw_box(&box, L'█', 0);

    char const * text = xre_state_get_string(text_state);
    ScreenCoordinates text_coords = {(screen_size->cols - strlen(text))/2, 2};
    text_coords.x = (screen_size->cols - strlen(text))/2;
    text_coords.y = 2;
    screen_printf(screen, &text_coords, "%s", text);
};


//------------------------------------------------------------------------------


void draw_transition(char const * title) {

    size_t title_len = strlen(title);

    ScreenSize const * screen_size = screen_get_size(screen);
    struct Box box = {
        .x = screen_size->cols / 4,
        .y = screen_size->rows / 2 - 2,
        .width = screen_size->cols / 2,
        .height = 5
    };

    draw_box(&box, L' ', 1);

    ScreenCoordinates text_coords = {
        (screen_size->cols - title_len) / 2,
        screen_size->rows/2
    };
    screen_printf(screen, &text_coords, "%s", title);
}


//------------------------------------------------------------------------------


void timer_screen_component(struct XREContext * ctx, va_list props) {
    (void) props;

    static char const title[] = "Timer Demo";

    ScreenSize const * screen_size = screen_get_size(screen);
    int pressed_key = use_pressed_key(ctx);
    struct Timer * timer = use_timer(ctx);
    ScreenCoordinates text_coords = {0, 0};

    text_coords.x = (screen_size->cols - strlen(title)) / 2;
    screen_printf(screen, &text_coords, "%s", title);

    if (timer_is_running(timer)) {
        double running_sec = timer_get_time_sec(timer);
        text_coords.y = 4;
        screen_printf(
            screen,
            &text_coords,
            "Timer: %fs of %fs.",
            running_sec,
            timer_get_time_sec(timer)
        );
    } else if (timer_is_done(timer)) {
        text_coords.y = 4;
        screen_printf(screen, &text_coords, "Timer is done!");
        timer_clear(timer);
    } else if (pressed_key != EOF) {
        timer_set(timer, 50.0);
    } else {
        text_coords.y = 9;
        screen_printf(screen, &text_coords, "Press any key");
    }

    double progress = timer_get_progress(timer);
    struct Box pbar_box = {(screen_size->cols - 14) / 2, 6, 10, 1};
    draw_box(&pbar_box, L' ', 1);
    for (size_t i = 0; i < pbar_box.width; i++) {
        double exact_width = progress * (double) pbar_box.width;
        if (i > exact_width) {
            continue;
        }

        ScreenCoordinates char_coords = {pbar_box.x + i, pbar_box.y};

        if (i + 1 <= exact_width) {
            screen_wprintf(screen, &char_coords, L"█");
            continue;
        }
        int diff = 8 * (exact_width - i);

        switch(diff) {
            case 1:
                screen_wprintf(screen, &char_coords, L"▏");
                break;
            case 2:
                screen_wprintf(screen, &char_coords, L"▎");
                break;
            case 3:
                screen_wprintf(screen, &char_coords, L"▍");
                break;
            case 4:
                screen_wprintf(screen, &char_coords, L"▌");
                break;
            case 5:
                screen_wprintf(screen, &char_coords, L"▋");
                break;
            case 6:
                screen_wprintf(screen, &char_coords, L"▊");
                break;
            case 7:
                screen_wprintf(screen, &char_coords, L"▉");
                break;
            default:
                break;
        }
    }
    text_coords.y = pbar_box.y;
    text_coords.x = pbar_box.x + pbar_box.width + 2;
    screen_printf(screen, &text_coords, "%d%%", (int)(100*progress));
}


//------------------------------------------------------------------------------


void ctx_destructor_screen_component(struct XREContext * ctx, va_list props) {
    struct XREContext * parent_ctx = va_arg(props, struct XREContext *);
    size_t children_count = va_arg(props, size_t);
    char const * const * children_titles = va_arg(props, char const * const *);

    static char const title[] = "App Context Deleter";

    struct XREStateSize * selected_index_state = xre_use_size(ctx, 0);
    struct XREStatePtr * selected_ctx_state = xre_use_ptr(ctx, NULL);
    struct XREStateChar * reload_state = xre_use_char(ctx, 0);
    size_t selected_index = xre_state_get_size(selected_index_state);
    ScreenSize const * screen_size = screen_get_size(screen);
    int pressed_key = use_pressed_key(ctx);

    if (pressed_key == 'k' && selected_index > 0) {
        selected_index--;
        xre_state_set_size(selected_index_state, selected_index);
    } else if (pressed_key == 'j' && selected_index + 1 < children_count) {
        selected_index++;
        xre_state_set_size(selected_index_state, selected_index);
    }

    if (
        xre_state_char_has_changed(reload_state)
        || xre_state_size_has_changed(selected_index_state)
    ) {
        char selected_key[12];
        snprintf(selected_key, 12, "%zu", selected_index);
        struct XREContext * child_ctx = xre_context_children_get(
            parent_ctx,
            selected_key
        );
        xre_state_set_ptr(selected_ctx_state, child_ctx);
    }

    ScreenCoordinates text_coords = {0, 4};
    text_coords.x = (screen_size->cols - strlen(title)) / 2;
    screen_printf(screen, &text_coords, "%s", title);
    text_coords.y += 2;

    text_coords.x -= 3;
    for (size_t i = 0; i < children_count; i++) {
        char const * title = children_titles[i];
        if (i == selected_index) {
            screen_printf(screen, &text_coords, "-> %zu. %s", i+1, title);
        } else {
            screen_printf(screen, &text_coords, "   %zu. %s", i+1, title);
        }
        text_coords.y++;
    }
    text_coords.y++;
    text_coords.x += 3;

    void * child_ctx = xre_state_get_ptr(selected_ctx_state);
    if (child_ctx == NULL) {
        screen_printf(screen, &text_coords, "No context found.");
        text_coords.y++;
    } else {
        screen_printf(screen, &text_coords, "Context: %p", child_ctx);
        text_coords.y++;
        screen_printf(screen, &text_coords, "Press enter to clear.");
    }

    if (pressed_key == '\n' && child_ctx != NULL) {
        char selected_key[12];
        snprintf(selected_key, 12, "%zu", selected_index);
        xre_context_child_destroy(parent_ctx, selected_key);

        xre_state_set_ptr(selected_ctx_state, NULL);
        char reload = xre_state_get_char(reload_state);
        reload++;
        xre_state_set_char(reload_state, reload);
    }
}


//------------------------------------------------------------------------------


void app(struct XREContext * ctx, va_list props) {

    static size_t const children_count = 6;
    static XREComponent const children[] = {
        title_screen_component,
        color_selector_screen_component,
        timer_screen_component,
        box_screen_component,
        box_screen_component,
        ctx_destructor_screen_component,
    };
    static char const * children_titles[] = {
        "Title",
        "Color Selector",
        "Timer",
        "Box 1",
        "Box 2",
        "Context Destructor",
    };
    static double const TRANSITION_DURATION = 0.5;

    int * should_exit = va_arg(props, int *);

    ScreenSize const * screen_size = screen_get_size(screen);
    double fps = use_fps(ctx, 0.5);
    ScreenCoordinates text_coords = {0, 0};
    int pressed_key = use_pressed_key(ctx);
    struct Timer * timer = use_timer(ctx);

    struct XREStateInt * child_index_state = xre_use_int(ctx, 0);
    int child_index = xre_state_get_int(child_index_state);

    if (pressed_key == 27) {
        *should_exit = 1;
    } else if (pressed_key == 'n') {
        child_index = (child_index + 1) % children_count;

        timer_set(timer, TRANSITION_DURATION);

        xre_state_set_int(child_index_state, child_index);
    } else if (pressed_key == 'p') {
        child_index = (child_index + children_count - 1) % children_count;

        timer_set(timer, TRANSITION_DURATION);

        xre_state_set_int(child_index_state, child_index);
    }

    if (strcmp(children_titles[child_index], "Context Destructor") == 0) {
        xre_use_ikey(
            ctx,
            child_index,
            children[child_index],
            ctx,
            children_count,
            children_titles
        );
    } else {
        xre_use_ikey(ctx, child_index, children[child_index]);
    }
    if (timer_is_running(timer)) {
        draw_transition(children_titles[child_index]);
    }

    draw_box(
        &(struct Box){1,1, screen_size->cols - 2, screen_size->rows - 2},
        L'\0',
        1
    );

    text_coords.x = 2;
    text_coords.y = screen_size->rows -1;
    screen_printf(screen, &text_coords, "╡ Press <ESC> to terminate. ╞");

    text_coords.x = screen_size->cols - 43;
    text_coords.y = screen_size->rows -1;
    screen_wprintf(screen, &text_coords, L"╡ Press 'p', 'n' to move among screens. ╞");

    text_coords.x = 2;
    text_coords.y = 0;
    screen_printf(screen, &text_coords, "╡ FPS: %d ╞", (int)fps);

    kbhit();
};


//------------------------------------------------------------------------------


int main(void) {

    double const SPF = 0.016;
    struct TerminalSize terminal_size = get_terminal_size();
    struct ScreenSize const screen_size = {
        .rows=terminal_size.rows,
        .cols=terminal_size.cols
    };

    kb_init();
    screen_init(&screen_size);
    struct XREContext * root_context = xre_context_root_alloc();

    int exit = 0;
    while (!exit) {
        xre_use_root(root_context, app, &exit);
        screen_render(screen);

        msleep((long int)(1000 * SPF));
    }
    screen_render_clear(screen);

    xre_context_destroy(root_context);
    screen_clean_up();
    kb_clean_up();
};
