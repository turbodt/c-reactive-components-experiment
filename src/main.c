#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include "xre.h"


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
//------------------------------------------------------------------------------

void text_renderer(struct IContext * ctx, void const * props) {
    (void) ctx;
    char const * text = props;
    printf("%s\n", text);
};

static struct IComponent * text = NULL;

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

struct TimeLoggerProps {
    time_t time;
    char const * header;
};

void time_logger_renderer(struct IContext * ctx, void const * props) {
    struct TimeLoggerProps const * p = props;

    struct IComponentRef * str_state = context_use_ref(
        ctx,
        (void *(*)(void))string_alloc,
        (void (*)(void *)) string_destroy
    );

    struct String * str = str_state->value;

    struct tm *local_time = localtime(&p->time);
    static char time_str[20];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", local_time);

    static char message[100] = "";
    snprintf(message, sizeof(message) - 1, "%s: %s", p->header, time_str);

    string_set(str, message);

    context_use(ctx, text, message);
}


static struct IComponent * time_logger = NULL;


//------------------------------------------------------------------------------

time_t * current_time_alloc(va_list args) {
    time_t * t = (time_t *) malloc(sizeof(time_t));
    *t = va_arg(args, time_t);
    return t;
};

void current_time_destroy(time_t *now) {
    free(now);
};

void app_renderer(struct IContext * ctx, void const *props) {
    (void) props;

    time_t now = time(NULL);
    struct IComponentRef * start_time_state = context_use_ref_ex(
        ctx,
        (void *(*)(va_list)) current_time_alloc,
        (void(*)(void *)) current_time_destroy,
        now
    );
    time_t * start_time = (time_t *) start_time_state->value;

    struct XREStateInt * cycle_cnt_state = xre_use_int(ctx, 0);

    int difftime_sec = (int) difftime(now, *start_time);

    if (difftime_sec >= 5) {
        *start_time = now;
        int cycle_cnt = xre_state_get_int(cycle_cnt_state);
        xre_state_set_int(cycle_cnt_state, cycle_cnt+1);
    }

    char message[100] = "";
    snprintf(
        message,
        sizeof(message) - 1,
        "Elapsed seconds: %d. Cycles: %d",
        difftime_sec,
        xre_state_get_int(cycle_cnt_state)
    );

    context_use(ctx, time_logger, &(struct TimeLoggerProps){.time=*start_time, .header="Initial time"});
    context_use(ctx, time_logger, &(struct TimeLoggerProps){.time=now, .header="Current time"});
    context_use(ctx, text, message);
    context_use(ctx, text, "Press any key to stop");
};


static struct IComponent * app = NULL;

//------------------------------------------------------------------------------

int main(void) {
    text = component_alloc("TEXT", text_renderer);
    time_logger = component_alloc("TIME_LOGGER", time_logger_renderer);
    app = component_alloc("APP", app_renderer);

    struct IContext * context = context_alloc(NULL);

    while (!kbhit()) {

        context_render_frame(context, app, NULL);

        usleep(16000);

        printf("\033[A\033[K");
        printf("\033[A\033[K");
        printf("\033[A\033[K");
        printf("\033[A\033[K");
    }
    getchar();

    context_destroy(context);

    component_destroy(app);
    component_destroy(time_logger);
    component_destroy(text);
};