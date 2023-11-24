#ifndef XRE_USE_H
#define XRE_USE_H


#include "./context.h"


void xre_use_root(struct IContext *, Component, ...);
void xre_vuse_root(struct IContext *, Component, va_list);

void xre_use(struct IContext *, char const *, Component, ...);
void xre_use_ikey(struct IContext *, int, Component, ...);
void xre_vuse(struct IContext *, char const *, Component, va_list);


#endif
