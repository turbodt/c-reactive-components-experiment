#include "./context_protected.h"
#include "./context_hierarchy_protected.h"
#include "./base.h"
#include <stdarg.h>
#include <string.h>


struct IContext * context_alloc(char const * key, Component component) {
    ContextPrivate * ctx = XRE_ALLOC(ContextPrivate, 1);

    ctx->key = NULL;

    if (!IS_NULL(key)) {
        size_t key_len = strlen(key);
        ctx->key = XRE_ALLOC(char, key_len + 1);
        ctx->key[key_len] = '\0';
        strncpy(ctx->key, key, key_len);
    }

    ctx->children = NULL;

    ctx->states_index = 0;
    ctx->states_size = 0;
    ctx->states = NULL;

    ctx->component = component;

    return (struct IContext *) ctx;
};


void context_destroy(struct IContext * context) {
    ContextPrivate * ctx = TO_CONTEXT_PRIV(context);

    context_children_destroy(ctx);

    for (size_t index = 0; index < ctx->states_size; index++) {
        context_state_destroy(ctx->states[index]);
        ctx->states[index] = NULL;
    }
    XRE_FREE(ctx->states);

    XRE_FREE(ctx->key);

    XRE_FREE(ctx);
};
