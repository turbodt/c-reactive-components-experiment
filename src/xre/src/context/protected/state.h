#ifndef XRE_CONTEXT_PROTECTED_STATE_H
#define XRE_CONTEXT_PROTECTED_STATE_H


#include "../state.h"


struct XREContextState {
    void * value;
    XREContextStateOnDestroy on_destroy;
};


#define TO_CTX_STATE_PRIVATE(state) (state)
#define TO_CTX_STATE_PUBLIC(state) (state)


void xre_context_state_clear(struct XREContextState *);


#endif
