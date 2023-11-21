#include "./use_ref.h"
#include "./context_protected.h"


struct IComponentRef * xre_use_vref(
    struct IContext *context,
    void *(*constructor)(va_list),
    void (*destructor)(void *),
    va_list constructor_args
) {
    ContextPrivate * ctx = TO_CONTEXT_PRIV(context);

    size_t index = ctx->states_index;
    ctx->states_index++;

    if (index >= ctx->states_size) {
        ctx->states = (struct IContextState **) realloc(
            ctx->states,
            (ctx->states_size + 1) * sizeof(struct IContext *)
        );
        ctx->states_size++;

        ctx->states[index] = context_state_alloc(
            constructor(constructor_args),
            destructor
        );
    }

    struct IContextState * state = ctx->states[index];

    // TODO: This is unsafe. Fix it.
    return (struct IComponentRef *) state;
};


struct IComponentRef * xre_use_ref(
    struct IContext *context,
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
