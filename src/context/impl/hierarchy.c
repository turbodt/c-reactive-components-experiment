#include "../hierarchy.h"
#include "../protected/context.h"
#include "../../base.h"


inline struct XREContext const * xre_context_children_get_const(
    struct XREContext const * context,
    char const * key
) {
    ContextPrivate const * ctx = TO_CONTEXT_PRIV(context);
    ContextPrivate * child;
    HASH_FIND_STR(ctx->children, key, child);
    return TO_CONTEXT_PUB(child);
};


inline struct XREContext * xre_context_children_get(
    struct XREContext * context,
    char const *key
) {
    ContextPrivate * ctx = TO_CONTEXT_PRIV(context);
    ContextPrivate * child;
    HASH_FIND_STR(ctx->children, key, child);
    return TO_CONTEXT_PUB(child);
}


inline void xre_context_children_add(
    struct XREContext * context,
    struct XREContext * child_context
) {
    ContextPrivate * ctx = TO_CONTEXT_PRIV(context);
    ContextPrivate * child = TO_CONTEXT_PRIV(child_context);
    HASH_ADD_KEYPTR(hh, ctx->children, child->key, strlen(child->key), child);
};


void xre_context_children_destroy(struct XREContext *context) {
    ContextPrivate * ctx = TO_CONTEXT_PRIV(context);
    ContextPrivate *child_context, *tmp;

    HASH_ITER(hh, ctx->children, child_context, tmp) {
        HASH_DEL(ctx->children, child_context);
        xre_context_destroy(TO_CONTEXT_PUB(child_context));
    }

    ctx->children = NULL;
};
