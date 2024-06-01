#ifndef XRE_USE_VALUE_USE_X_FACTORY_H
#define XRE_USE_VALUE_USE_X_FACTORY_H


#include "../context/main.h"
#include "../use_ref.h"


#ifndef XRE_CONFIG_NOT_STRUCT_ALIAS

#define XRE_USE_X_FACTORY_H(type, struct_type, name) \
struct struct_type { \
    struct XRERef ref; \
}; \
typedef struct struct_type struct_type ; \
 \
struct struct_type * xre_use_##name(struct XREContext *, type); \
type xre_state_get_##name(struct struct_type *); \
void xre_state_set_##name(struct struct_type *, type); \
int xre_state_##name##_has_changed(struct struct_type const *); // TODO int -> XRE_BOOL

#else

#define XRE_USE_X_FACTORY_H(type, struct_type, name) \
struct struct_type { \
    struct XRERef ref; \
}; \
 \
 \
struct struct_type * xre_use_##name(struct XREContext *, type); \
type xre_state_get_##name(struct struct_type *); \
void xre_state_set_##name(struct struct_type *, type); \
int xre_state_##name##_has_changed(struct struct_type const *); // TODO int -> XRE_BOOL

#endif

#endif
