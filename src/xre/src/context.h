#ifndef XRE_CONTEXT_H
#define XRE_CONTEXT_H


#include "./context_state.h"
#include "./component.h"
#include <stdlib.h>


struct IComponentRef {
    void * value;
};


struct IContext;


struct IContext * context_alloc(struct IComponent *);
void context_destroy(struct IContext *);


void context_render_frame(struct IContext *, struct IComponent *, void const *);

void context_use(struct IContext *, struct IComponent *, void const *);

struct IComponentRef context_use_ref(
    struct IContext *,
    void *(*)(void),
    void (*)(void *)
);

#endif
