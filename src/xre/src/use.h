#ifndef XRE_USE_H
#define XRE_USE_H


#include "./context_protected.h"


void context_render_frame(struct IContext *, Component, ...);
void context_vrender_frame(struct IContext *, Component, va_list);

void xre_use(struct IContext *, Component, ...);
void xre_vuse(struct IContext *, Component, va_list);


#endif
