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


#endif
