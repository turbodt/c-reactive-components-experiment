#include "./context_protected.h"
#include "./base.h"
#include <stdarg.h>


static void component_render(struct IContext *, Component, va_list);


struct IContext * context_alloc(Component component) {
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
    ContextPrivate * ctx = TO_CONTEXT_PRIV(context);

    for (size_t index = 0; index < ctx->children_size; index++) {
        struct IContext * child = TO_CONTEXT_PUB(ctx->children[index]);
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
    Component component,
    ...
) {
    va_list props;
    va_start(props, component);
    context_vrender_frame(parent_context, component, props);
    va_end(props);
};


void context_vrender_frame(
    struct IContext *parent_context,
    Component component,
    va_list props
) {
    ContextPrivate * pctx = TO_CONTEXT_PRIV(parent_context);

    pctx->children_index = 0;
    pctx->states_index = 0;

    xre_vuse(parent_context, component, props);
};


void xre_vuse(
    struct IContext *parent_context,
    Component component,
    va_list props
) {

    ContextPrivate * pctx = TO_CONTEXT_PRIV(parent_context);

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
    ContextPrivate * ctx = TO_CONTEXT_PRIV(pctx->children[index]);
    ctx->children_index = 0;
    ctx->states_index = 0;

    component_render(TO_CONTEXT_PUB(ctx), component, props);
};


void xre_use(
    struct IContext *parent_context,
    Component component,
    ...
) {
    va_list props;
    va_start(props, component);
    xre_vuse(parent_context, component, props);
    va_end(props);
};


//--------------------------------------------------------------------------
//


inline void component_render(
    struct IContext *ctx,
    Component component,
    va_list props
) {
    component(ctx, props);
};
