#include "./context_protected.h"
#include "./context_hierarchy_protected.h"
#include "./base.h"
#include <stdarg.h>
#include <string.h>


struct XREContext * xre_context_alloc(char const * key, Component component) {
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

    return (struct XREContext *) ctx;
};


inline struct XREContext * xre_context_root_alloc(void) {
    return xre_context_alloc(NULL, NULL);
};


void xre_context_destroy(struct XREContext * context) {
    ContextPrivate * ctx = TO_CONTEXT_PRIV(context);

    xre_context_children_destroy(ctx);

    for (size_t index = 0; index < ctx->states_size; index++) {
        xre_context_state_destroy(ctx->states[index]);
        ctx->states[index] = NULL;
    }
    XRE_FREE(ctx->states);

    XRE_FREE(ctx->key);

    XRE_FREE(ctx);
};
