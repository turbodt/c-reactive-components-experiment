#include "./context_state.h"
#include "./base.h"


struct IContextState {};
typedef struct {
    struct IContextState base;
    void * value;
    void (*destructor)(void *);
} ContextStatePrivate;


#define TO_PRIV(state) ((ContextStatePrivate *)(state))
#define TO_PUB(state) ((struct IContextState *)(state))



struct IContextState * context_state_alloc(
    void * value,
    void (*destructor)(void *)
) {
    ContextStatePrivate * state = XRE_ALLOC(ContextStatePrivate, 1);

    state->value = value;
    state->destructor = destructor;

    return TO_PUB(state);
};


void context_state_destroy(struct IContextState *istate) {
    ContextStatePrivate * state = TO_PRIV(istate);

    if (!IS_NULL(state->destructor)) {
        state->destructor(state->value);
        state->destructor = NULL;
    }

    XRE_FREE(state);
};


void * context_state_get(struct IContextState *istate) {
    ContextStatePrivate * state = TO_PRIV(istate);
    return state->value;
};


void context_state_set(struct IContextState *istate, void *value) {
    ContextStatePrivate * state = TO_PRIV(istate);
    state->value = value;
};
