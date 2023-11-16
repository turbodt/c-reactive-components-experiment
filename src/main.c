#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>


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


struct IComponent;
struct IContext;

struct IContextState {
    void * value;
    void (*destructor)(void *);
};


struct IComponentState {
    void * value;
};


struct IContext {
    size_t children_index;
    size_t children_size;
    struct IContext **children;

    size_t states_index;
    size_t states_size;
    struct IContextState **states;
    struct IComponentState (*use_state)(
        struct IContext *,
        void *(*)(void),
        void (*)(void *)
    );

    void(*render)(struct IContext *, struct IComponent *, void *);
};


struct ContextPrivate {
    struct IContext base;
    struct IComponent * component;
};


struct IComponent {
    char const * type;
    void(*make)(struct IContext *, void *);
};

struct ComponentPrivate {
    struct IComponent base;
    void(*renderer)(struct IContext *, void const *);
};


//------------------------------------------------------------------------------


void component_make(struct IContext *cxt, void *props) {
    (void) cxt;
    (void) props;
};

struct IComponent * component_alloc(
    char const * type,
    void(*renderer)(struct IContext *, void const *)
) {
    struct ComponentPrivate * component = (struct ComponentPrivate *) malloc(
        sizeof(struct ComponentPrivate)
    );

    component->renderer = renderer;
    component->base.type = type;
    component->base.make = component_make;

    return (struct IComponent *) component;
};


void component_destroy(struct IComponent * component) {
    free(component);
}


//------------------------------------------------------------------------------

struct IContextState * context_state_alloc(
    void * value,
    void (*destructor)(void *)
) {
    struct IContextState * state = (struct IContextState *) malloc(
        sizeof(struct IContextState)
    );

    state->value = value;
    state->destructor = destructor;

    return state;
};


void context_state_destroy(struct IContextState *state) {
    state->destructor(state->value);
    free(state);
};


//------------------------------------------------------------------------------


static struct IContext * context_alloc(struct IComponent *);


void context_render(
    struct IContext *parent_cxt,
    struct IComponent * component,
    void *props
) {
    struct ComponentPrivate * cpt = (struct ComponentPrivate *) (component);

    size_t index = parent_cxt->children_index;
    parent_cxt->children_index++;

    if (index >= parent_cxt->children_size) {
        parent_cxt->children = (struct IContext **) realloc(
            parent_cxt->children,
            (parent_cxt->children_size + 1) * sizeof(struct IContext *)
        );
        parent_cxt->children_size++;

        parent_cxt->children[index] = context_alloc(component);
    }
    struct IContext * cxt = parent_cxt->children[index];
    cxt->children_index = 0;
    cxt->states_index = 0;

    cpt->renderer(cxt, props);
};


struct IComponentState context_use_state(
    struct IContext *cxt,
    void *(*constructor)(void),
    void (*destructor)(void *)
) {
    size_t index = cxt->states_index;
    cxt->states_index++;

    if (index >= cxt->states_size) {
        cxt->states = (struct IContextState **) realloc(
            cxt->states,
            (cxt->states_size + 1) * sizeof(struct IContext *)
        );
        cxt->states_size++;

        cxt->states[index] = context_state_alloc(constructor(), destructor);
    }

    struct IContextState * state = cxt->states[index];

    return (struct IComponentState){
        .value=state->value,
    };
};


struct IContext * context_alloc(struct IComponent *component) {
    struct ContextPrivate * cxt = (struct ContextPrivate *) malloc(
        sizeof(struct ContextPrivate)
    );

    cxt->base.children_index = 0;
    cxt->base.children_size = 0;
    cxt->base.children = NULL;
    cxt->base.states_index = 0;
    cxt->base.states_size = 0;
    cxt->base.states = NULL;
    cxt->base.use_state = context_use_state;
    cxt->base.render = context_render;
    cxt->component = component;

    return (struct IContext *) cxt;
};

void context_destroy(struct IContext * cxt) {
    for (size_t index = 0; index < cxt->children_size; index++) {
        context_destroy(cxt->children[index]);
        cxt->children[index] = NULL;
    }
    free(cxt->children);
    cxt->children = NULL;

    for (size_t index = 0; index < cxt->states_size; index++) {
        context_state_destroy(cxt->states[index]);
        cxt->states[index] = NULL;
    }
    free(cxt->states);
    cxt->states = NULL;

    free(cxt);
};

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

    struct IComponentState str_state = ctx->use_state(
        ctx,
        (void *(*)(void))string_alloc,
        (void (*)(void *)) string_destroy
    );

    struct String * str = str_state.value;

    struct tm *local_time = localtime(&p->time);
    static char time_str[20];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", local_time);

    static char message[100] = "";
    snprintf(message, sizeof(message) - 1, "%s: %s", p->header, time_str);

    string_set(str, message);

    ctx->render(ctx, text, message);
}


static struct IComponent * time_logger = NULL;


//------------------------------------------------------------------------------

time_t * current_time_alloc(void) {
    time_t * now = (time_t *) malloc(sizeof(time_t));
    *now = time(NULL);
    return now;
};

void current_time_destroy(time_t *now) {
    free(now);
};

void app_renderer(struct IContext * ctx, void const *props) {
    (void) props;

    struct IComponentState start_time_state = ctx->use_state(
        ctx,
        (void *(*)(void)) current_time_alloc,
        (void(*)(void *)) current_time_destroy
    );
    time_t * start_time = (time_t *) start_time_state.value;

    time_t now = time(NULL);
    int difftime_sec = (int) difftime(now, *start_time);

    if (difftime_sec > 5) {
        *start_time = now;
    }

    char message[100] = "";
    snprintf(message, sizeof(message) - 1, "Elapsed seconds: %d", difftime_sec);

    ctx->render(ctx, time_logger, &(struct TimeLoggerProps){.time=*start_time, .header="Initial time"});
    ctx->render(ctx, time_logger, &(struct TimeLoggerProps){.time=now, .header="Current time"});
    ctx->render(ctx, text, message);
    ctx->render(ctx, text, "Press any key to stop");
};


static struct IComponent * app = NULL;

//------------------------------------------------------------------------------

int main(void) {
    text = component_alloc("TEXT", text_renderer);
    time_logger = component_alloc("TIME_LOGGER", time_logger_renderer);
    app = component_alloc("APP", app_renderer);

    struct IContext * context = context_alloc(NULL);

    while (!kbhit()) {

        context->states_index = 0;
        context->children_index = 0;
        context->render(context, app, NULL);

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
