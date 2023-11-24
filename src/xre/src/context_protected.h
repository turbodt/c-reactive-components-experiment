#ifndef XRE_CONTEXT_PRIVATE_H
#define XRE_CONTEXT_PRIVATE_H


#include "./context.h"
#include <uthash.h>


struct IContext {};
struct ContextPrivate {
    struct IContext base;
    char * key;

    struct ContextPrivate *children;

    size_t states_index;
    size_t states_size;
    struct IContextState **states;
    Component component;

    UT_hash_handle hh;
};
typedef struct ContextPrivate ContextPrivate;


#define TO_CONTEXT_PRIV(context) ((ContextPrivate *)(context))
#define TO_CONTEXT_PUB(context) ((struct IContext *)(context))


#define BOOL char
#define TRUE 1
#define FALSE 1
#define NOT(b) (1-(b))


BOOL context_children_has(ContextPrivate *, char const *);
ContextPrivate * context_children_get(ContextPrivate *, char const *);
void context_children_add(ContextPrivate *, ContextPrivate *);


#endif
