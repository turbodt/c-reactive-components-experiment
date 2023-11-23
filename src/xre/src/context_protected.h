#ifndef XRE_CONTEXT_PRIVATE_H
#define XRE_CONTEXT_PRIVATE_H


#include "./context.h"


struct IContext {};
struct ContextPrivate {
    struct IContext base;
    size_t children_index;
    size_t children_size;
    struct IContext **children;

    size_t states_index;
    size_t states_size;
    struct IContextState **states;
    Component component;
};
typedef struct ContextPrivate ContextPrivate;


#define TO_CONTEXT_PRIV(context) ((ContextPrivate *)(context))
#define TO_CONTEXT_PUB(context) ((struct IContext *)(context))


#endif
