#ifndef XRE_USE_VALUE_IMPL_USE_X_FACTORY_H
#define XRE_USE_VALUE_IMPL_USE_X_FACTORY_H


#include "../use_x_factory.h"
#include "../../context/main.h"
#include "../../use_ref.h"
#include "../../base.h"
#include <stdarg.h>


#define XRE_USE_X_FACTORY_IMPL_EX(type, struct_type, promoted_type, name, constructor, destructor, assignator, comparator) \
\
struct struct_type * xre_use_##name(struct XREContext * ctx, type initial_value) { \
    struct XRERef * ref = xre_use_ref_ex( \
        ctx, \
        constructor, \
        destructor, \
        assignator, \
        comparator, \
        initial_value \
    ); \
    return (struct struct_type *) ref; \
}; \
 \
 \
inline type xre_state_get_##name(struct struct_type *state) { \
    return *((type *) xre_ref_get((struct XRERef *)state)); \
}; \
 \
 \
inline void xre_state_set_##name(struct struct_type *state, type value) { \
    xre_ref_set((struct XRERef *)state, &value); \
} \
 \
 \
inline XRE_BOOL xre_state_##name##_has_changed(struct struct_type const *state) { \
    return xre_ref_has_changed((struct XRERef *)state); \
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
static void * xre_##name##_constructor(va_list args) { \
    type * value = XRE_ALLOC(type, 1); \
    XRE_ASSERT_ALLOC(value); \
    type new_value = (type) va_arg(args, promoted_type); \
    xre_##name##_assignator(value, &new_value); \
    return value; \
}; \
 \
 \
static void xre_##name##_destructor(void *value) { \
    XRE_FREE(value); \
}; \
 \
 \
  XRE_USE_X_FACTORY_IMPL_EX( \
    type, \
    struct_type, \
    promoted_type, \
    name, \
    xre_##name##_constructor, \
    xre_##name##_destructor, \
    xre_##name##_assignator, \
    xre_##name##_comparator \
  )

#endif
