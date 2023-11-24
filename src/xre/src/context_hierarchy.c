#include "./context_hierarchy_protected.h"
#include "./context_protected.h"
#include "./base.h"


inline ContextPrivate * context_children_get(
    ContextPrivate const *ctx,
    char const *key
) {
    ContextPrivate * child;
    HASH_FIND_STR(ctx->children, key, child);
    return child;
}

inline struct XREContext const * context_children_get_ro(
    struct XREContext const * context,
    char const * key
) {
    ContextPrivate const * ctx = TO_CONTEXT_PRIV(context);
    ContextPrivate const * child = context_children_get(ctx, key);
    return TO_CONTEXT_PUB(child);
};


inline void context_children_add(ContextPrivate * ctx, ContextPrivate * child) {
    HASH_ADD_KEYPTR(hh, ctx->children, child->key, strlen(child->key), child);
};


void context_children_destroy(ContextPrivate *context) {
    ContextPrivate *child_context, *tmp;

    HASH_ITER(hh, context->children, child_context, tmp) {
        HASH_DEL(context->children, child_context);
        context_destroy(TO_CONTEXT_PUB(child_context));
    }

    context->children = NULL;
};
