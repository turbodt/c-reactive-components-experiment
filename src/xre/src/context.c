#include "./context_protected.h"
#include "./base.h"
#include <stdarg.h>


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
