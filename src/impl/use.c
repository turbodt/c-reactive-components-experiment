#include "../use.h"
#include "../context/main.h"
#include "../base.h"
#include <stdio.h>


static char const XRE_ROOT_KEY[] = "ROOT";


static void component_call(struct XREContext *, XREComponent, va_list);


void xre_vuse(
    struct XREContext *parent_context,
    char const * key,
    XREComponent component,
    va_list props
) {


    struct XREContext * ctx = xre_context_children_get(parent_context, key);
    if (IS_NULL(ctx)) {
        ctx = xre_context_alloc(key, component);
        xre_context_children_add(parent_context, ctx);
    }

    xre_context_state_reset_index(ctx);

    component_call(ctx, component, props);
};


void xre_use(
    struct XREContext *parent_context,
    char const * key,
    XREComponent component,
    ...
) {
    va_list props;
    va_start(props, component);
    xre_vuse(parent_context, key, component, props);
    va_end(props);
};


void xre_use_ikey(
    struct XREContext *parent_context,
    int key,
    XREComponent component,
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
    struct XREContext *parent_context,
    XREComponent component,
    va_list props
) {
    xre_context_state_reset_index(parent_context);

    xre_vuse(parent_context, XRE_ROOT_KEY, component, props);
};


void xre_use_root(
    struct XREContext *parent_context,
    XREComponent component,
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
    struct XREContext *ctx,
    XREComponent component,
    va_list props
) {
    component(ctx, props);
};
