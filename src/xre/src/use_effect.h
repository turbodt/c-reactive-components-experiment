#ifndef XRE_USE_EFFECT_H
#define XRE_USE_EFFECT_H


#include "./context/main.h"
#include "./use_ref.h"
#include <stdarg.h>


typedef void (*XREEffectCleanUp)(va_list);
typedef void (*XREEffect)(va_list); // TODO: return clean up


struct XREEffectRef {
    struct XRERef base;
};


struct XREEffectRef * xre_use_veffect(
    struct XREContext *,
    XREEffect,
    struct XRERef const * const *,
    va_list
);


struct XREEffectRef * xre_use_effect(
    struct XREContext *,
    XREEffect,
    struct XRERef const * const *,
    ...
);


#endif
