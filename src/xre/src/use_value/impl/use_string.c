#include "../use_string.h"
#include "../../context/main.h"
#include "../../use_ref.h"
#include "../../base.h"
#include <stdlib.h>
#include <string.h>


typedef struct XREString {
    char * value;
    size_t len;
} XREString;


#define AS_XRE_STRING(ptr) ((struct XREString *)(ptr))


static void * constructor(va_list);
static void destructor(void *);
static void assignator(void *, void const *);
static int comparator(void const *, void const *);


struct XREStateString * xre_use_string(struct XREContext * ctx, char const * initial_value) {
    struct XRERef * ref = xre_use_ref_ex(
        ctx,
        constructor,
        destructor,
        assignator,
        comparator,
        initial_value
    );
    return (struct XREStateString *) ref;
};


inline char const * xre_state_get_string(struct XREStateString *state) {
    return ((XREString *) xre_ref_get((struct XRERef *)state))->value;
};


inline void xre_state_set_string(struct XREStateString *state, char const * value) {
    xre_ref_set((struct XRERef *)state, &value);
}


inline XRE_BOOL xre_state_string_has_changed(struct XREStateString const *state) {
    return xre_ref_has_changed((struct XRERef *)state);
}


//------------------------------------------------------------------------------
// Private
//------------------------------------------------------------------------------


void * constructor(va_list args) {
    XREString * xre_string = XRE_ALLOC(XREString, 1);
    xre_string->value = NULL;
    xre_string->len = 0;

    char const * new_value = (char const *) va_arg(args, char const *);
    assignator(xre_string, &new_value);

    return xre_string;
};


void destructor(void *xre_string) {
    XRE_FREE(AS_XRE_STRING(xre_string)->value);
    AS_XRE_STRING(xre_string)->len = 0;
    XRE_FREE(xre_string);
};


void assignator(void *dst, void const *src) {
    XREString * str_dst = AS_XRE_STRING(dst);
    char const * const * p_src = (char const * const *) src;

    size_t len_src = strlen(*p_src);

    if (
        IS_NULL(str_dst->value)
        || str_dst->len < len_src
        || str_dst->len > 2*len_src
    ) {
        str_dst->value = XRE_REALLOC(str_dst->value, char, len_src + 1);
    }

    strncpy(str_dst->value, *p_src, len_src + 1);
};


int comparator(void const *curr, void const *new) {
    XREString * str_curr = AS_XRE_STRING(curr);
    char const * const * p_new = (char const * const *) new;
    return strcmp(str_curr->value, *p_new);
};
