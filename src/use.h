#ifndef XRE_USE_H
#define XRE_USE_H


#include "./context/main.h"


void xre_use_root(struct XREContext *, XREComponent, ...);
void xre_vuse_root(struct XREContext *, XREComponent, va_list);

void xre_use(struct XREContext *, char const *, XREComponent, ...);
void xre_use_ikey(struct XREContext *, int, XREComponent, ...);
void xre_vuse(struct XREContext *, char const *, XREComponent, va_list);


#endif
