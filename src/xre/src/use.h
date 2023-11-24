#ifndef XRE_USE_H
#define XRE_USE_H


#include "./context.h"


void xre_use_root(struct XREContext *, Component, ...);
void xre_vuse_root(struct XREContext *, Component, va_list);

void xre_use(struct XREContext *, char const *, Component, ...);
void xre_use_ikey(struct XREContext *, int, Component, ...);
void xre_vuse(struct XREContext *, char const *, Component, va_list);


#endif
