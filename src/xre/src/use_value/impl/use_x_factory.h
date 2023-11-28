#ifndef XRE_USE_VALUE_IMPL_USE_X_FACTORY_H
#define XRE_USE_VALUE_IMPL_USE_X_FACTORY_H


#include "../use_x_factory.h"
#include "../../context/main.h"
#include "../../use_ref.h"
#include "../../base.h"
#include <stdarg.h>


#define XRE_USE_X_FACTORY_IMPL_EX(type, struct_type, promoted_type, name, clean_up, assignator, comparator) \
\
struct struct_type { \
    type value; \
    struct XRERef * ref; \
}; \
 \
 \
static void * xre_##name##_state_alloc(va_list args) { \
    struct struct_type * state = XRE_ALLOC(struct struct_type, 1); \
    XRE_ASSERT_ALLOC(state); \
    type new_value = (type) va_arg(args, promoted_type); \
    assignator(&state->value, &new_value); \
    return &state->value; \
}; \
 \
 \
static void xre_##name##_state_destroy(void *state) { \
    if (!IS_NULL(clean_up)) { \
        clean_up(state); \
    } \
    XRE_FREE(state); \
}; \
 \
 \
struct struct_type * xre_use_##name(struct XREContext * ctx, type initial_value) { \
    struct XRERef * ref = xre_use_ref_ex( \
        ctx, \
        xre_##name##_state_alloc, \
        xre_##name##_state_destroy, \
        assignator, \
        comparator, \
        initial_value \
    ); \
    struct struct_type * state = (struct struct_type *) xre_ref_get(ref); \
    state->ref = ref; \
    return state; \
}; \
 \
 \
inline type xre_state_get_##name(struct struct_type *state) { \
    return *((type *) xre_ref_get(state->ref)); \
}; \
 \
 \
inline void xre_state_set_##name(struct struct_type *state, type value) { \
    xre_ref_set(state->ref, &value); \
} \
 \
 \
inline XRE_BOOL xre_state_##name##_has_changed(struct struct_type const *state) { \
    return xre_ref_has_changed(state->ref); \
}


#define XRE_USE_X_FACTORY_IMPL(type, struct_type, promoted_type, name) \
 \
 \
static inline void xre_##name##_assignator(void * dst, void const * src) { \
    *(type *)dst = *(type const *)src; \
}; \
 \
 \
static inline XRE_BOOL xre_##name##_comparator(void const * dst, void const * src) { \
    return *(type const *)dst == *(type const *)src ? 0 : 1; \
}; \
 \
 \
  XRE_USE_X_FACTORY_IMPL_EX(type, struct_type, promoted_type, name, ((void(*)(void *))NULL), xre_##name##_assignator, xre_##name##_comparator)

#endif
