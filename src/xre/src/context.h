#ifndef XRE_CONTEXT_H
#define XRE_CONTEXT_H


#include "./context_state.h"
#include <stdlib.h>
#include <stdarg.h>



struct IComponentRef {
    void * value;
};


struct IContext;


typedef void (*Component)(struct IContext *, va_list);


struct IContext * context_alloc(Component);
void context_destroy(struct IContext *);


void context_render_frame(struct IContext *, Component, ...);
void context_vrender_frame(struct IContext *, Component, va_list);

void xre_use(struct IContext *, Component, ...);
void xre_vuse(struct IContext *, Component, va_list);


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
