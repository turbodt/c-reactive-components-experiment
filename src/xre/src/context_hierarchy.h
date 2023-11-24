#ifndef XRE_CONTEXT_HIERARCHY_H
#define XRE_CONTEXT_HIERARCHY_H


#include "./context.h"


struct XREContext const * context_children_get_ro(
    struct XREContext const *,
    char const *
);


#endif
