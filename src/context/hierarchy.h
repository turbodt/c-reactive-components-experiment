#ifndef XRE_CONTEXT_HIERARCHY_H
#define XRE_CONTEXT_HIERARCHY_H


#include "./context.h"


struct XREContext const * xre_context_children_get_const(
    struct XREContext const *,
    char const *
);
struct XREContext * xre_context_children_get(struct XREContext *, char const *);
void xre_context_children_add(struct XREContext *, struct XREContext *);
void xre_context_children_destroy(struct XREContext *);


#endif
