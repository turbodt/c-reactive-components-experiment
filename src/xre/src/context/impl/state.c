#include "../protected/state.h"
#include "../../base.h"


inline void const * xre_context_state_get_const(
    struct XREContextState const *istate
) {
    struct XREContextState const * state = TO_CTX_STATE_PRIVATE(istate);
    return state->value;
};


inline void * xre_context_state_get(struct XREContextState *istate) {
    struct XREContextState * state = TO_CTX_STATE_PRIVATE(istate);
    return state->value;
};


inline void xre_context_state_set(struct XREContextState *istate, void *value) {
    struct XREContextState * state = TO_CTX_STATE_PRIVATE(istate);
    state->value = value;
};


inline void xre_context_state_clear(struct XREContextState *istate) {
    struct XREContextState * state = TO_CTX_STATE_PRIVATE(istate);
    if (!IS_NULL(state->on_destroy)) {
        state->on_destroy(state);
    }

    state->on_destroy = NULL;
    state->value = NULL;
};
