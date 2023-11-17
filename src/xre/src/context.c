#include "./context.h"
#include "./base.h"


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


static void context_render(struct IContext *, struct IComponent *, void *);
static struct IComponentState context_use_state(
    struct IContext *,
    void *(*constructor)(void),
    void (*destructor)(void *)
);


struct IContext * context_alloc(struct IComponent *component) {
    ContextPrivate * ctx = XRE_ALLOC(ContextPrivate, 1);

    ctx->children_index = 0;
    ctx->children_size = 0;
    ctx->children = NULL;
    ctx->states_index = 0;
    ctx->states_size = 0;
    ctx->states = NULL;

    ctx->base.use_state = &context_use_state;
    ctx->base.render = &context_render;
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
    void *props
) {
    ContextPrivate * pctx = TO_PRIV(parent_context);

    pctx->children_index = 0;
    pctx->states_index = 0;

    context_render(parent_context, component, props);
};


//------------------------------------------------------------------------------
// Private
//------------------------------------------------------------------------------


void context_render(
    struct IContext *parent_context,
    struct IComponent * component,
    void *props
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


struct IComponentState context_use_state(
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

    return CLITERAL(struct IComponentState){
        .value=state->value,
    };
};
