#include "./context.h"
#include "./base.h"
#include <stdarg.h>


struct IContext {};
struct ContextPrivate {
    struct IContext base;
    size_t children_index;
    size_t children_size;
    struct IContext **children;

    size_t states_index;
    size_t states_size;
    struct IContextState **states;
    struct IComponent * component;
};
typedef struct ContextPrivate ContextPrivate;


#define TO_PRIV(context) ((ContextPrivate *)(context))
#define TO_PUB(context) ((struct IContext *)(context))


struct IContext * context_alloc(struct IComponent *component) {
    ContextPrivate * ctx = XRE_ALLOC(ContextPrivate, 1);

    ctx->children_index = 0;
    ctx->children_size = 0;
    ctx->children = NULL;
    ctx->states_index = 0;
    ctx->states_size = 0;
    ctx->states = NULL;

    ctx->component = component;

    return (struct IContext *) ctx;
};


void context_destroy(struct IContext * context) {
    ContextPrivate * ctx = TO_PRIV(context);

    for (size_t index = 0; index < ctx->children_size; index++) {
        struct IContext * child = TO_PUB(ctx->children[index]);
        context_destroy(child);
        ctx->children[index] = NULL;
    }
    XRE_FREE(ctx->children);

    for (size_t index = 0; index < ctx->states_size; index++) {
        context_state_destroy(ctx->states[index]);
        ctx->states[index] = NULL;
    }
    XRE_FREE(ctx->states);

    XRE_FREE(ctx);
};


void context_render_frame(
    struct IContext *parent_context,
    struct IComponent * component,
    void const *props
) {
    ContextPrivate * pctx = TO_PRIV(parent_context);

    pctx->children_index = 0;
    pctx->states_index = 0;

    context_use(parent_context, component, props);
};


struct IComponentRef * context_use_ref(
    struct IContext *context,
    void *(*constructor)(void),
    void (*destructor)(void *)
) {
    ContextPrivate * ctx = TO_PRIV(context);

    size_t index = ctx->states_index;
    ctx->states_index++;

    if (index >= ctx->states_size) {
        ctx->states = (struct IContextState **) realloc(
            ctx->states,
            (ctx->states_size + 1) * sizeof(struct IContext *)
        );
        ctx->states_size++;

        ctx->states[index] = context_state_alloc(constructor(), destructor);
    }

    struct IContextState * state = ctx->states[index];

    // TODO: This is unsafe. Fix it.
    return (struct IComponentRef *) state;
};


struct IComponentRef * context_use_vref(
    struct IContext *context,
    void *(*constructor)(va_list),
    void (*destructor)(void *),
    va_list constructor_args
) {
    ContextPrivate * ctx = TO_PRIV(context);

    size_t index = ctx->states_index;
    ctx->states_index++;

    if (index >= ctx->states_size) {
        ctx->states = (struct IContextState **) realloc(
            ctx->states,
            (ctx->states_size + 1) * sizeof(struct IContext *)
        );
        ctx->states_size++;

        ctx->states[index] = context_state_alloc(
            constructor(constructor_args),
            destructor
        );
    }

    struct IContextState * state = ctx->states[index];

    // TODO: This is unsafe. Fix it.
    return (struct IComponentRef *) state;
};



struct IComponentRef * context_use_ref_ex(
    struct IContext *context,
    void *(*constructor)(va_list),
    void (*destructor)(void *),
    ...
) {
    va_list constructor_args;
    va_start(constructor_args, destructor);
    struct IComponentRef * ref = context_use_vref(
        context,
        constructor,
        destructor,
        constructor_args
    );
    va_end(constructor_args);

    return ref;
};


void context_use(
    struct IContext *parent_context,
    struct IComponent * component,
    void const *props
) {

    ContextPrivate * pctx = TO_PRIV(parent_context);

    size_t index = pctx->children_index;
    pctx->children_index++;

    if (index >= pctx->children_size) {
        pctx->children = (struct IContext **) realloc(
            pctx->children,
            (pctx->children_size + 1) * sizeof(struct IContext *)
        );
        pctx->children_size++;

        pctx->children[index] = context_alloc(component);
    }
    ContextPrivate * ctx = TO_PRIV(pctx->children[index]);
    ctx->children_index = 0;
    ctx->states_index = 0;

    component_render(component, TO_PUB(ctx), props);
};


//--------------------------------------------------------------------------
//
