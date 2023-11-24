#ifndef XRE_USE_VALUE_USE_X_FACTORY_H
#define XRE_USE_VALUE_USE_X_FACTORY_H


#include "../context.h"
#include "../use_ref.h"
#include "../base.h"
#include <stdarg.h>


struct XREState {
    void * value;
};


#define XRE_USE_X_FACTORY_H(type, struct_type, name) \
struct struct_type; \
 \
struct struct_type * xre_use_##name(struct XREContext *, type); \
type xre_state_get_##name(struct struct_type *); \
void xre_state_set_##name(struct struct_type *, type);


#define XRE_USE_X_FACTORY_EX(type, struct_type, promoted_type, name, assign) \
\
struct struct_type { \
    struct XREState base; \
}; \
 \
 \
static void * name##_state_alloc(va_list args) { \
    struct struct_type * state = XRE_ALLOC(struct struct_type, 1); \
    state->base.value = XRE_ALLOC(type, 1); \
    type new_value = (type) va_arg(args, promoted_type); \
    assign(state->base.value, &new_value); \
    return state; \
}; \
 \
static void name##_state_destroy(void *state) { \
    XRE_FREE(((struct struct_type *) state)->base.value); \
    XRE_FREE(state); \
}; \
\
\
struct struct_type * xre_use_##name(struct XREContext * ctx, type initial_value) { \
    return (struct struct_type *) xre_use_ref( \
        ctx, \
        name##_state_alloc, \
        name##_state_destroy, \
        initial_value \
    ); \
}; \
 \
 \
inline type xre_state_get_##name(struct struct_type *state) { \
    return *((type *) state->base.value); \
}; \
 \
 \
inline void xre_state_set_##name(struct struct_type *state, type value) { \
    type * ptr = (type *) state->base.value; \
    *ptr = value; \
    assign(ptr, &value); \
}


#define XRE_USE_X_FACTORY(type, struct_type, promoted_type, name) \
 \
 \
static inline void name##_assign(type * dst, type const * src) { \
    *dst = *src; \
} \
 \
 \
  XRE_USE_X_FACTORY_EX(type, struct_type, promoted_type, name, name##_assign)

#endif
