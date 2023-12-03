#include "./use.h"
#include "./context/main.h"
#include "./base.h"
#include <stdio.h>


static char const XRE_ROOT_KEY[] = "ROOT";


static void component_call(struct XREContext *, Component, va_list);


void xre_vuse(
    char const * key,
    Component component,
    struct XREContext *parent_context,
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
    char const * key,
    Component component,
    struct XREContext *parent_context,
    ...
) {
    va_list props;
    va_start(props, parent_context);
    xre_vuse(key, component, parent_context, props);
    va_end(props);
};


void xre_use_ikey(
    int key,
    Component component,
    struct XREContext *parent_context,
    ...
) {
    char key_str[12];

    sprintf(key_str, "%d", key);

    va_list props;
    va_start(props, parent_context);
    xre_vuse(key_str, component, parent_context, props);
    va_end(props);
};


void xre_vuse_root(
    Component component,
    struct XREContext *parent_context,
    va_list props
) {
    xre_context_state_reset_index(parent_context);

    xre_vuse(XRE_ROOT_KEY, component, parent_context, props);
};


void xre_use_root(
    Component component,
    struct XREContext *parent_context,
    ...
) {
    va_list props;
    va_start(props, parent_context);
    xre_vuse_root(component, parent_context, props);
    va_end(props);
};


//--------------------------------------------------------------------------
// Private
//--------------------------------------------------------------------------


inline void component_call(
    struct XREContext *ctx,
    Component component,
    va_list props
) {
    component(ctx, props);
};
