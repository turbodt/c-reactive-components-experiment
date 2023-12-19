#ifndef XRE_USE_EFFECT_H
#define XRE_USE_EFFECT_H


#include "./context/main.h"
#include "./use_ref.h"
#include <stdarg.h>


struct XREEffectCleanUp;
typedef void (*XREEffectCleanUpFunction)(void *);
typedef struct XREEffectCleanUp * (*XREEffect)(va_list);


struct XREEffectRef {
    struct XRERef ref;
};


struct XREEffectRef * xre_use_veffect(
    struct XREContext *,
    XREEffect,
    struct XRERef const * const[],
    va_list
);


struct XREEffectRef * xre_use_effect(
    struct XREContext *,
    XREEffect,
    struct XRERef const * const[],
    ...
);


struct XREEffectCleanUp * xre_effect_clean_up_alloc(
    XREEffectCleanUpFunction,
    void *
);


#endif
