#ifndef XRE_USE_VALUE_USE_X_FACTORY_H
#define XRE_USE_VALUE_USE_X_FACTORY_H


#include "../context/main.h"


#define XRE_USE_X_FACTORY_H(type, struct_type, name) \
struct struct_type; \
 \
struct struct_type * xre_use_##name(struct XREContext *, type); \
type xre_state_get_##name(struct struct_type *); \
void xre_state_set_##name(struct struct_type *, type); \
int xre_state_##name##_has_changed(struct struct_type const *); // TODO int -> XRE_BOOL


#endif
