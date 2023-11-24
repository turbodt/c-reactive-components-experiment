#include "./use.h"
#include "./context_protected.h"
#include "./base.h"
#include <stdio.h>


static char const XRE_ROOT_KEY[] = "ROOT";


static void component_call(struct IContext *, Component, va_list);


void xre_vuse(
    struct IContext *parent_context,
    char const * key,
    Component component,
    va_list props
) {

    ContextPrivate * pctx = TO_CONTEXT_PRIV(parent_context);

    ContextPrivate * ctx = context_children_get(pctx, key);
    if (IS_NULL(ctx)) {
        ctx = TO_CONTEXT_PRIV(context_alloc(key, component));
        context_children_add(pctx, ctx);
    }

    ctx->states_index = 0;

    component_call(TO_CONTEXT_PUB(ctx), component, props);
};


void xre_use(
    struct IContext *parent_context,
    char const * key,
    Component component,
    ...
) {
    va_list props;
    va_start(props, component);
    xre_vuse(parent_context, key, component, props);
    va_end(props);
};


void xre_use_ikey(
    struct IContext *parent_context,
    int key,
    Component component,
    ...
) {
    char key_str[12];

    sprintf(key_str, "%d", key);

    va_list props;
    va_start(props, component);
    xre_vuse(parent_context, key_str, component, props);
    va_end(props);
};


void xre_vuse_root(
    struct IContext *parent_context,
    Component component,
    va_list props
) {
    ContextPrivate * pctx = TO_CONTEXT_PRIV(parent_context);

    pctx->states_index = 0;

    xre_vuse(parent_context, XRE_ROOT_KEY, component, props);
};


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
