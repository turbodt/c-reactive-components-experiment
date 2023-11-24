#include "./context_protected.h"
#include "./base.h"
#include <stdarg.h>
#include <string.h>


static void context_children_destroy(ContextPrivate *);


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


inline BOOL context_children_has(ContextPrivate *ctx, char const *key) {
    return IS_NULL(context_children_get(ctx, key));
};


inline ContextPrivate * context_children_get(ContextPrivate *ctx, char const *key) {
    ContextPrivate * child;
    HASH_FIND_STR(ctx->children, key, child);
    return child;
}


inline void context_children_add(ContextPrivate * ctx, ContextPrivate * child) {
    HASH_ADD_KEYPTR(hh, ctx->children, child->key, strlen(child->key), child);
};


//------------------------------------------------------------------------------
// PRIVATE
//------------------------------------------------------------------------------


void context_children_destroy(ContextPrivate *context) {
    ContextPrivate *child_context, *tmp;

    HASH_ITER(hh, context->children, child_context, tmp) {
        HASH_DEL(context->children, child_context);
        context_destroy(TO_CONTEXT_PUB(child_context));
    }

    context->children = NULL;
};
