#ifndef XRE_CONTEXT_PROTECTED_H
#define XRE_CONTEXT_PROTECTED_H


#include "../context.h"
#include "../state.h"
#include <uthash.h>


struct XREContext {};
struct ContextPrivate {
    struct XREContext base;
    char * key;

    struct ContextPrivate *children;

    size_t states_index;
    size_t states_count;
    struct XREContextState *states;
    XREComponent component;

    UT_hash_handle hh;
};
typedef struct ContextPrivate ContextPrivate;


#define TO_CONTEXT_PRIV(context) ((ContextPrivate *)(context))
#define TO_CONTEXT_PUB(context) ((struct XREContext *)(context))


#endif
