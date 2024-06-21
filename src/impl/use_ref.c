#include "../use_ref.h"
#include "../context/main.h"
#include "../base.h"


#define TO_REF_PRIVATE(ref) ref
#define TO_REF_PUBLIC(ref) ref


#define CTX_STATE_TO_REF(state) ((struct XRERef *)xre_context_state_get(state))
#define REF_TO_CTX_STATE(ref) ((ref)->state)


static struct XRERef * xre_ref_alloc(
    XRERefConstructor,
    XRERefDestructor,
    XRERefAssignator,
    XRERefComparator,
    va_list
);
static void xre_ref_destroy(struct XRERef *);
static void on_destroy(struct XREContextState *);


struct XRERef * xre_use_vref(
    struct XREContext *context,
    XRERefConstructor constructor,
    XRERefDestructor destructor,
    XRERefAssignator assignator,
    XRERefComparator comparator,
    va_list constructor_args
) {
    struct XREContextState * state = xre_context_get_next_state(context);

    if (IS_NULL(state)) {
        struct XRERef * ref = xre_ref_alloc(
            constructor,
            destructor,
            assignator,
            comparator,
            constructor_args
        );

        ref->state = xre_context_append_new_state(context, ref, on_destroy);
        state = xre_context_get_next_state(context);
        assert(ref->state == state);
    } else {
        CTX_STATE_TO_REF(state)->has_changed = 0;
    };

    return CTX_STATE_TO_REF(state);
};


struct XRERef * xre_use_ref(
    struct XREContext *context,
    XRERefConstructor constructor,
    XRERefDestructor destructor,
    ...
) {
    va_list constructor_args;
    va_start(constructor_args, destructor);
    struct XRERef * ref = xre_use_vref(
        context,
        constructor,
        destructor,
        NULL,
        NULL,
        constructor_args
    );
    va_end(constructor_args);

    return ref;
};


struct XRERef * xre_use_ref_ex(
    struct XREContext *context,
    XRERefConstructor constructor,
    XRERefDestructor destructor,
    XRERefAssignator assignator,
    XRERefComparator comparator,
    ...
) {
    va_list constructor_args;
    va_start(constructor_args, comparator);
    struct XRERef * ref = xre_use_vref(
        context,
        constructor,
        destructor,
        assignator,
        comparator,
        constructor_args
    );
    va_end(constructor_args);

    return ref;
};


inline void const * xre_ref_get_const(struct XRERef const *ref) {
    return ref->value;
};


inline void * xre_ref_get(struct XRERef * ref) {
    return ref->value;
};


inline void xre_ref_set(struct XRERef * ref, void * value) {
    XRE_BOOL are_different = xre_ref_cmp(ref, value);
    if (IS_NULL(ref->assignator)) {
        ref->value = value;
    } else {
        ref->assignator(ref->value, value);
    }
    ref->has_changed = are_different;
};


inline XRE_BOOL xre_ref_cmp(struct XRERef const * ref, void const * value) {
    void const * current_value = xre_ref_get_const(ref);
    if (IS_NULL(ref->comparator)) {
        return current_value == value ? 0 : 1;
    } else {
        return ref->comparator(current_value, value);
    }
};


inline XRE_BOOL xre_ref_has_changed(struct XRERef const * ref) {
    return ref->has_changed;
};


//------------------------------------------------------------------------------
// Private
//------------------------------------------------------------------------------


struct XRERef * xre_ref_alloc(
    XRERefConstructor constructor,
    XRERefDestructor destructor,
    XRERefAssignator assignator,
    XRERefComparator comparator,
    va_list constructor_args
) {
    struct XRERef * ref = XRE_ALLOC(struct XRERef, 1);
    XRE_ASSERT_ALLOC(ref);

    ref->destructor = destructor;
    ref->assignator = assignator;
    ref->comparator = comparator;
    ref->value = NULL;
    ref->state = NULL;
    ref->has_changed = 1;

    ref->value = constructor(constructor_args); // TODO: use xre_ref_set instead?

    return ref;
};


static void xre_ref_destroy(struct XRERef * ref) {
    if (!IS_NULL(ref->destructor)) {
        ref->destructor(ref->value);
    }
    XRE_FREE(ref);
};


static void on_destroy(struct XREContextState * state) {
    struct XRERef * ref = CTX_STATE_TO_REF(state);
    xre_ref_destroy(ref);
    xre_context_state_set(state, NULL);
};
