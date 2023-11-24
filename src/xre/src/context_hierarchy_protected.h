#ifndef XRE_CONTEXT_HIERARCHY_PROTECTED_H
#define XRE_CONTEXT_HIERARCHY_PROTECTED_H


#include "./context_hierarchy.h"
#include "./context_protected.h"


ContextPrivate * xre_context_children_get(ContextPrivate const *, char const *);
void xre_context_children_add(ContextPrivate *, ContextPrivate *);
void xre_context_children_destroy(ContextPrivate *context);


#endif
