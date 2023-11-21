#ifndef XRE_USE_VALUE_USE_X_FACTORY_H
#define XRE_USE_VALUE_USE_X_FACTORY_H


#include "../context.h"
#include "../use_ref.h"
#include "../base.h"
#include <stdarg.h>


struct XREState {
    struct IComponentRef base;
};


#define XRE_USE_X_FACTORY_H(type, struct_type, name) \
struct struct_type; \
 \
struct struct_type * xre_use_##name(struct IContext *, type); \
type xre_state_get_##name(struct struct_type *); \
void xre_state_set_##name(struct struct_type *, type);


#define XRE_USE_X_FACTORY_EX(type, struct_type, promoted_type, name, assign) \
\
struct struct_type { \
    struct XREState base; \
}; \
 \
 \
static void * name##_alloc(va_list args) { \
    type * value = XRE_ALLOC(type, 1); \
    type new_value = (type) va_arg(args, promoted_type); \
    assign(value, &new_value); \
    return value; \
}; \
 \
static void name##_destroy(void *value) { \
    XRE_FREE(value); \
}; \
\
\
struct struct_type * xre_use_##name(struct IContext * ctx, type initial_value) { \
    struct IComponentRef * ref = xre_use_ref( \
        ctx, \
        name##_alloc, \
        name##_destroy, \
        initial_value \
    ); \
    return (struct struct_type *) ref; \
}; \
 \
 \
inline type xre_state_get_##name(struct struct_type *state) { \
    return *((type *) state->base.base.value); \
}; \
 \
 \
inline void xre_state_set_##name(struct struct_type *state, type value) { \
    type * ptr = (type *) state->base.base.value; \
    *ptr = value; \
    assign(ptr, &value); \
}


#define XRE_USE_X_FACTORY(type, struct_type, promoted_type, name) \
 \
 \
static inline void type##_assign(type * dst, type const * src) { \
    *dst = *src; \
} \
 \
 \
  XRE_USE_X_FACTORY_EX(type, struct_type, promoted_type, name, type##_assign)

#endif
