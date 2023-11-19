#ifndef XRE_USE_VALUE_H
#define XRE_USE_VALUE_H


#include "./context.h"


struct XREState;


#define XRE_USE_X_FACTORY_H(type, struct_type, name) \
struct struct_type; \
 \
struct struct_type * xre_use_##name(struct IContext *, type); \
type xre_state_get_##name(struct struct_type *); \
void xre_state_set_##name(struct struct_type *, type);


XRE_USE_X_FACTORY_H(char, XREStateChar, char)
XRE_USE_X_FACTORY_H(int, XREStateInt, int)
XRE_USE_X_FACTORY_H(double, XREStateDouble, double)
XRE_USE_X_FACTORY_H(float, XREStateFloat, float)

#endif
