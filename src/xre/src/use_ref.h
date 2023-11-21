#ifndef XRE_USE_REF_H
#define XRE_USE_REF_H


#include "./context.h"


struct IComponentRef * xre_use_vref(
    struct IContext *,
    void *(*)(va_list),
    void (*)(void *),
    va_list
);


struct IComponentRef * xre_use_ref(
    struct IContext *,
    void *(*)(va_list),
    void (*)(void *),
    ...
);


#endif
