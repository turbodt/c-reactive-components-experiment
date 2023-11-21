#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include "xre.h"
#include "./use_time.h"
#include "./use_timespec.h"


int kbhit(void) {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if(ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

int msleep(long int msec)
{
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}
//------------------------------------------------------------------------------

void text(struct IContext * ctx, va_list props) {
    (void) ctx;
    char const * format = va_arg(props, char const *);
    vprintf(format, props);
};

//------------------------------------------------------------------------------

struct String {
    size_t size;
    char *value;
};

struct String * string_alloc(void) {
    struct String * str = (struct String *) malloc(sizeof(struct String));
    str->value = NULL;
    str->size = 0;
    return str;
};

void string_destroy(struct String * str) {
    if (str->value != NULL) {
        free(str->value);
        str->value = NULL;
    }
    str->size = 0;
    free(str);
}

void string_set(struct String *dst, char const *src) {
    size_t src_size = strlen(src) + 1;
    if (dst->size < src_size || dst->size > 2 * src_size) {
        dst->value = (char *) realloc(dst->value, src_size * sizeof(char));
        dst->size = src_size;
    }
    snprintf(dst->value, dst->size -1, "%s", src);
};


void time_logger(struct IContext * ctx, va_list props) {
    time_t time = va_arg(props, time_t);
    char const * header = va_arg(props, char const *);

    struct tm *local_time = localtime(&time);
    static char time_str[20];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", local_time);

    xre_use(ctx, text, "%s: %s\n", header, time_str);
}


//------------------------------------------------------------------------------


void last_key_pressed(struct IContext *ctx, va_list props) {
    (void) props;

    struct XREStateChar * last_pressed = xre_use_char(ctx, '\0');

    if (kbhit()) {
        char c = getchar();
        xre_state_set_char(last_pressed, c);
    }

    char c = xre_state_get_char(last_pressed);

    if (c == '\0') {
        xre_use(ctx, text, "No key pressed yet.\n");
    } else {
        xre_use(ctx, text, "Last key pressed is '%c' ('%d').\n", c, (int)c);
    }
}


//------------------------------------------------------------------------------

void fps(struct IContext * ctx, va_list props) {
    (void) props;

    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    struct StateTimeSpec * last_time_state = xre_use_timespec(ctx, now);
    struct timespec last_time = xre_state_get_timespec(last_time_state);

    long seconds = now.tv_sec - last_time.tv_sec;
    long ndiff = ((seconds * 1000000000L) + now.tv_nsec) - last_time.tv_nsec;

    if (ndiff < 100) {
        xre_use(ctx, text, "Elapsed time: %ldns\n", ndiff);
        return;
    }

    double fps = 1000000000.0/ndiff;
    xre_state_set_timespec(last_time_state, now);

    xre_use(ctx, text, "FPS: %f\n", fps);
};

//------------------------------------------------------------------------------

void app(struct IContext * ctx, va_list props) {
    (void) props;

    time_t now = time(NULL);
    struct StateTime * start_time_state = xre_use_time(ctx, now);

    time_t start_time = xre_state_get_time(start_time_state);

    struct XREStateInt * cycle_cnt_state = xre_use_int(ctx, 0);

    int difftime_sec = (int) difftime(now, start_time);

    if (difftime_sec >= 5) {
        xre_state_set_time(start_time_state, now);
        start_time = xre_state_get_time(start_time_state);

        int cycle_cnt = xre_state_get_int(cycle_cnt_state);
        xre_state_set_int(cycle_cnt_state, cycle_cnt+1);
    }

    xre_use(ctx, fps);
    xre_use(ctx, time_logger, start_time, "Initial time");
    xre_use(ctx, time_logger, now, "Current time");
    xre_use(
        ctx,
        text,
        "Elapsed seconds: %d. Cycles: %d\n",
        difftime_sec,
        xre_state_get_int(cycle_cnt_state)
    );
    xre_use(ctx, last_key_pressed);
    xre_use(ctx, text, "Press <ESC> to stop\n");
};


//------------------------------------------------------------------------------

int main(void) {
    struct IContext * context = context_alloc(NULL);

    double const SPF = 0.016;

    time_t sleep_start = 0.0;
    time_t sleep_end = 0.0;
    int exit = 0;
    while (!exit) {
        printf("Last sleep time: %f\n", difftime(sleep_end, sleep_start));

        time_t render_start = time(NULL);
        context_render_frame(context, app);
        time_t render_end = time(NULL);

        double elapsed_time = difftime(render_end, render_start);

        sleep_start = time(NULL);
        if (elapsed_time < SPF) {
            msleep((long int)(1000 * (SPF - elapsed_time)));
            //sleep(1);
        }
        sleep_end = time(NULL);

        if (kbhit()) {
            char c = getchar();
            exit = c == 27;
            ungetc(c, stdin);
        }

        if (!exit) {
            printf("\033[A\033[K");
            printf("\033[A\033[K");
            printf("\033[A\033[K");
            printf("\033[A\033[K");
            printf("\033[A\033[K");
            printf("\033[A\033[K");
            printf("\033[A\033[K");
        }
    }

    context_destroy(context);
};
