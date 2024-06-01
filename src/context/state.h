#ifndef XRE_CONTEXT_STATE_H
#define XRE_CONTEXT_STATE_H


#include <stdlib.h>


struct XREContextState;


typedef void (*XREContextStateOnDestroy)(struct XREContextState *);


void const * xre_context_state_get_const(struct XREContextState const *);
void * xre_context_state_get(struct XREContextState *);
void xre_context_state_set(struct XREContextState *, void *);


#ifndef XRE_CONFIG_NOT_STRUCT_ALIAS
typedef struct XREContextState XREContextState;
#endif


#endif
