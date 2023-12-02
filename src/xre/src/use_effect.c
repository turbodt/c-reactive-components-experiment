#include "./use_effect.h"
#include "./base.h"
#include <stdarg.h>


#define FALSE 0
#define TRUE 1


static void * xre_effect_constructor(va_list);
static void xre_effect_destructor(void *);


struct XREEffectRef * xre_use_veffect(
    struct XREContext *ctx,
    XREEffect effect,
    struct XRERef const * const dependencies[],
    va_list effect_args
) {
    struct XRERef * ref = xre_use_ref(
        ctx,
        xre_effect_constructor,
        xre_effect_destructor
    );

    size_t index = 0;
    XRE_BOOL has_changed = FALSE;
    while (has_changed == FALSE && !IS_NULL(dependencies[index])) {
        has_changed = xre_ref_has_changed(dependencies[index]);
    }

    if (has_changed) {
        effect(effect_args);
    }

    return (struct XREEffectRef *) ref;
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


//------------------------------------------------------------------------------
// Private
//------------------------------------------------------------------------------


inline void * xre_effect_constructor(va_list args) {
    return NULL;
};

inline void xre_effect_destructor(void * value) {

};
