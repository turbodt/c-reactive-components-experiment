#ifndef XRE_CONTEXT_STATE_H
#define XRE_CONTEXT_STATE_H


#include <stdlib.h>


struct IContextState {
    void * value;
};

struct IContextState * context_state_alloc(void *, void (*)(void *));
void context_state_destroy(struct IContextState *);


void * context_state_get(struct IContextState *);
void context_state_set(struct IContextState *, void *);


#endif
