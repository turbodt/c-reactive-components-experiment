#ifndef XRE_CONTEXT_STATE_H
#define XRE_CONTEXT_STATE_H


#include <stdlib.h>


struct XREContextState;

struct XREContextState * context_state_alloc(void *, void (*)(void *));
void context_state_destroy(struct XREContextState *);


void * context_state_get(struct XREContextState *);
void context_state_set(struct XREContextState *, void *);


#endif
