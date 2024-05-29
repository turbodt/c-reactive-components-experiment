#include "../protected/context.h"
#include "../protected/state.h"
#include "../hierarchy.h"
#include "../../base.h"
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
    ctx->states_count = 0;
    ctx->states = NULL;

    ctx->component = component;

    return (struct XREContext *) ctx;
};


inline struct XREContext * xre_context_root_alloc(void) {
    return xre_context_alloc(NULL, NULL);
};


void xre_context_destroy(struct XREContext * context) {
    ContextPrivate * ctx = TO_CONTEXT_PRIV(context);

    xre_context_children_destroy(context);

    for (size_t index = ctx->states_count; index > 0; index--) {
        xre_context_state_clear(&ctx->states[index-1]);
    }
    XRE_FREE(ctx->states);

    XRE_FREE(ctx->key);

    XRE_FREE(ctx);
};


inline void xre_context_state_reset_index(struct XREContext * context) {
    ContextPrivate * ctx = TO_CONTEXT_PRIV(context);
    ctx->states_index = 0;
};


struct XREContextState * xre_context_get_next_state(
    struct XREContext * context
) {
    ContextPrivate * ctx = TO_CONTEXT_PRIV(context);

    if (ctx->states_index >= ctx->states_count) {
        return NULL;
    }
    size_t index = ctx->states_index;
    ctx->states_index++;
    return &ctx->states[index];
};


struct XREContextState * xre_context_append_new_state(
    struct XREContext * context,
    void * value,
    XREContextStateOnDestroy on_destroy
) {
    ContextPrivate * ctx = TO_CONTEXT_PRIV(context);

    size_t next_index = ctx->states_count;
    ctx->states = XRE_REALLOC(
        ctx->states,
        struct XREContextState,
        (ctx->states_count + 1)
    );
    XRE_ASSERT_ALLOC(ctx->states);
    ctx->states_count++;

    struct XREContextState *state = &ctx->states[next_index];

    state->value = value;
    state->on_destroy = on_destroy;

    return state;
};
