#ifndef XRE_CONTEXT_HIERARCHY_H
#define XRE_CONTEXT_HIERARCHY_H


#include "./context.h"


struct IContext const * context_children_get_ro(
    struct IContext const *,
    char const *
);


#endif
