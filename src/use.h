#ifndef XRE_USE_H
#define XRE_USE_H


#include "./context/main.h"


void xre_use_root(struct XREContext *, XREComponent, ...);
void xre_vuse_root(struct XREContext *, XREComponent, va_list);

void xre_use(char const *, struct XREContext *, XREComponent, ...);
void xre_use_ikey(int, struct XREContext *, XREComponent, ...);
void xre_vuse(char const *, struct XREContext *, XREComponent, va_list);


#endif
