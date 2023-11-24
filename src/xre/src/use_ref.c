#include "./use_ref.h"
#include "./context_protected.h"


struct IComponentRef * xre_use_vref(
    struct XREContext *context,
    void *(*constructor)(va_list),
    void (*destructor)(void *),
    va_list constructor_args
) {
    ContextPrivate * ctx = TO_CONTEXT_PRIV(context);

    size_t index = ctx->states_index;
    ctx->states_index++;

    if (index >= ctx->states_size) {
        ctx->states = (struct XREContextState **) realloc(
            ctx->states,
            (ctx->states_size + 1) * sizeof(struct XREContext *)
        );
        ctx->states_size++;

        ctx->states[index] = context_state_alloc(
            constructor(constructor_args),
            destructor
        );
    }

    struct XREContextState * state = ctx->states[index];

    // TODO: This is unsafe. Fix it.
    return (struct IComponentRef *) state;
};


struct IComponentRef * xre_use_ref(
    struct XREContext *context,
    void *(*constructor)(va_list),
    void (*destructor)(void *),
    ...
) {
    va_list constructor_args;
    va_start(constructor_args, destructor);
    struct IComponentRef * ref = xre_use_vref(
        context,
        constructor,
        destructor,
        constructor_args
    );
    va_end(constructor_args);

    return ref;
};
