#ifndef XRE_USE_H
#define XRE_USE_H


#include "./context/main.h"


void xre_use_root(XREComponent, struct XREContext *, ...);
void xre_vuse_root(XREComponent, struct XREContext *, va_list);

void xre_use(char const *, XREComponent, struct XREContext *, ...);
void xre_use_ikey(int, XREComponent, struct XREContext *, ...);
void xre_vuse(char const *, XREComponent, struct XREContext *, va_list);


#endif
