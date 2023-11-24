#ifndef XRE_CONTEXT_H
#define XRE_CONTEXT_H


#include "./context_state.h"
#include <stdlib.h>
#include <stdarg.h>



struct IComponentRef {
    void * value;
};


struct XREContext;


typedef void (*Component)(struct XREContext *, va_list);


struct XREContext * xre_context_alloc(char const *, Component);
struct XREContext * xre_context_root_alloc(void);
void xre_context_destroy(struct XREContext *);


#endif
