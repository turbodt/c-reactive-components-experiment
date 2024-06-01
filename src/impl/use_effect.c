#include "../use_effect.h"
#include "../base.h"
#include <stdarg.h>


#define FALSE 0
#define TRUE 1


typedef struct XREEffectCleanUp {
    XREEffectCleanUpFunction fn;
    void * props;
} XREEffectCleanUp;


typedef struct EffectRefValue {
    XREEffect effect;
    XREEffectCleanUp * clean_up;
} EffectRefValue;


static void xre_effect_clean_up_destroy(XREEffectCleanUp *);
static void * xre_effect_constructor(va_list);
static void xre_effect_destructor(void *);
static void xre_effect_assignator(void *, void const *);
static XRE_BOOL xre_effect_comparator(void const *, void const *);
static void call_effect(struct XREEffectRef *, va_list);
static void call_and_destroy_clean_up(EffectRefValue const *);
static void xre_effect_set_effect(struct XREEffectRef *, XREEffect);
static void xre_effect_set_clean_up(
    struct XREEffectRef *,
    XREEffectCleanUp *
);


struct XREEffectRef * xre_use_veffect(
    struct XREContext *ctx,
    XREEffect effect,
    struct XRERef const * const dependencies[],
    va_list effect_args
) {
    struct XREEffectRef * effect_ref = (struct XREEffectRef *) xre_use_ref_ex(
        ctx,
        xre_effect_constructor,
        xre_effect_destructor,
        xre_effect_assignator,
        xre_effect_comparator,
        effect
    );

    EffectRefValue const * curr_value = xre_ref_get_const(&effect_ref->ref);
    if (effect != curr_value->effect) {
        xre_effect_set_effect(effect_ref, effect);
    }

    size_t index = 0;
    XRE_BOOL deps_have_changed = IS_NULL(dependencies)
        || xre_ref_has_changed(&effect_ref->ref);
    while (!deps_have_changed && !IS_NULL(dependencies[index])) {
        deps_have_changed = xre_ref_has_changed(dependencies[index]);
        index++;
    }

    if (deps_have_changed) {
        call_effect(effect_ref, effect_args);
    }

    return effect_ref;
};


struct XREEffectRef * xre_use_effect(
    struct XREContext *ctx,
    XREEffect effect,
    struct XRERef const * const dependencies[],
    ...
) {
    va_list effect_args;
    va_start(effect_args, dependencies);
    struct XREEffectRef * ref = xre_use_veffect(
        ctx,
        effect,
        dependencies,
        effect_args
    );
    va_end(effect_args);

    return ref;
};


XREEffectCleanUp * xre_effect_clean_up_alloc(
    XREEffectCleanUpFunction clean_up_fn,
    void * props
) {
    XREEffectCleanUp * clean_up = XRE_ALLOC(XREEffectCleanUp, 1);
    XRE_ASSERT_ALLOC(clean_up);
    clean_up->fn = clean_up_fn;
    clean_up->props = props;
    return clean_up;
};


//------------------------------------------------------------------------------
// Private
//------------------------------------------------------------------------------


inline void xre_effect_clean_up_destroy(XREEffectCleanUp * clean_up) {
    RETURN_IF_NULL(clean_up);
    clean_up->fn = NULL;
    clean_up->props = NULL;
    XRE_FREE(clean_up);
};


inline void * xre_effect_constructor(va_list args) {
    EffectRefValue * value = XRE_ALLOC(EffectRefValue, 1);
    XRE_ASSERT_ALLOC(value);

    value->effect = va_arg(args, XREEffect);
    value->clean_up = NULL;

    return value;
};

inline void xre_effect_destructor(void * value) {
    EffectRefValue * curr_value = (EffectRefValue *) value;
    curr_value->effect = NULL;
    call_and_destroy_clean_up(curr_value);
    curr_value->clean_up = NULL;
    XRE_FREE(value);
};


inline void xre_effect_assignator(void *dst, void const *src) {
    EffectRefValue * curr_value = (EffectRefValue *) dst;
    EffectRefValue * new_value = (EffectRefValue *) src;

    curr_value->effect = new_value->effect;
    curr_value->clean_up = new_value->clean_up;
};


inline XRE_BOOL xre_effect_comparator(void const *curr, void const *new) {
    EffectRefValue * curr_value = (EffectRefValue *) curr;
    EffectRefValue * new_value = (EffectRefValue *) new;

    return (
        curr_value->clean_up == new_value->clean_up
        && curr_value->effect == new_value->effect
    ) ? 0 : 1;
};


inline void call_effect(struct XREEffectRef * effect_ref, va_list args) {
    EffectRefValue * value = (EffectRefValue *) xre_ref_get(&effect_ref->ref);

    call_and_destroy_clean_up(value);
    value->clean_up = NULL;

    XREEffectCleanUp * clean_up = NULL;

    if (!IS_NULL(value->effect)) {
        clean_up = value->effect(args);
    }

    xre_effect_set_clean_up(effect_ref, clean_up);
};


inline void call_and_destroy_clean_up(EffectRefValue const * value) {
    if (IS_NULL(value->clean_up)) {
        return;
    }
    value->clean_up->fn(value->clean_up->props);
    xre_effect_clean_up_destroy(value->clean_up);
};


inline void xre_effect_set_effect(
    struct XREEffectRef *effect_ref,
    XREEffect effect
) {
    EffectRefValue * value = (EffectRefValue *) xre_ref_get(&effect_ref->ref);
    EffectRefValue new_value = {
        .effect=effect,
        .clean_up=value->clean_up,
    };
    xre_ref_set(&effect_ref->ref, &new_value);
};


inline void xre_effect_set_clean_up(
    struct XREEffectRef *effect_ref,
    XREEffectCleanUp * clean_up
) {
    EffectRefValue * value = (EffectRefValue *) xre_ref_get(&effect_ref->ref);
    EffectRefValue new_value = {
        .effect=value->effect,
        .clean_up=clean_up,
    };
    xre_ref_set(&effect_ref->ref, &new_value);
};
