#ifndef XRE_USE_REF_H
#define XRE_USE_REF_H


#include "./context.h"


void * xre_use_vref(
    struct XREContext *,
    void *(*)(va_list),
    void (*)(void *),
    va_list
);


void * xre_use_ref(
    struct XREContext *,
    void *(*)(va_list),
    void (*)(void *),
    ...
);


#endif
