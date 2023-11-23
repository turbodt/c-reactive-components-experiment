#include "./use.h"
#include "./context_protected.h"
#include "./base.h"


static void component_call(struct IContext *, Component, va_list);


void xre_use_root(
    struct IContext *parent_context,
    Component component,
    ...
) {
    va_list props;
    va_start(props, component);
    xre_vuse_root(parent_context, component, props);
    va_end(props);
};


void xre_vuse_root(
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

    component_call(TO_CONTEXT_PUB(ctx), component, props);
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
// Private
//--------------------------------------------------------------------------


inline void component_call(
    struct IContext *ctx,
    Component component,
    va_list props
) {
    component(ctx, props);
};