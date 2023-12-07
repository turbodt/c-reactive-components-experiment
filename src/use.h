#ifndef XRE_USE_H
#define XRE_USE_H


#include "./context/main.h"


void xre_use_root(Component, struct XREContext *, ...);
void xre_vuse_root(Component, struct XREContext *, va_list);

void xre_use(char const *, Component, struct XREContext *, ...);
void xre_use_ikey(int, Component, struct XREContext *, ...);
void xre_vuse(char const *, Component, struct XREContext *, va_list);


#endif
