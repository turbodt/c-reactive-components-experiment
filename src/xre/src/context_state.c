#include "./context_state.h"
#include "./base.h"


struct XREContextState {};
typedef struct {
    struct XREContextState base;
    void * value;
    void (*destructor)(void *);
} ContextStatePrivate;


#define TO_PRIV(state) ((ContextStatePrivate *)(state))
#define TO_PUB(state) ((struct XREContextState *)(state))



struct XREContextState * context_state_alloc(
    void * value,
    void (*destructor)(void *)
) {
    ContextStatePrivate * state = XRE_ALLOC(ContextStatePrivate, 1);

    state->value = value;
    state->destructor = destructor;

    return TO_PUB(state);
};


void context_state_destroy(struct XREContextState *istate) {
    ContextStatePrivate * state = TO_PRIV(istate);

    if (!IS_NULL(state->destructor)) {
        state->destructor(state->value);
        state->destructor = NULL;
    }

    XRE_FREE(state);
};


void * context_state_get(struct XREContextState *istate) {
    ContextStatePrivate * state = TO_PRIV(istate);
    return state->value;
};


void context_state_set(struct XREContextState *istate, void *value) {
    ContextStatePrivate * state = TO_PRIV(istate);
    state->value = value;
};
