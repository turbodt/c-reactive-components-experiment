#ifndef XRE_CONTEXT_PRIVATE_H
#define XRE_CONTEXT_PRIVATE_H


#include "./context.h"
#include <uthash.h>


struct XREContext {};
struct ContextPrivate {
    struct XREContext base;
    char * key;

    struct ContextPrivate *children;

    size_t states_index;
    size_t states_size;
    struct XREContextState **states;
    Component component;

    UT_hash_handle hh;
};
typedef struct ContextPrivate ContextPrivate;


#define TO_CONTEXT_PRIV(context) ((ContextPrivate *)(context))
#define TO_CONTEXT_PUB(context) ((struct XREContext *)(context))


#endif
