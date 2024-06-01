#ifndef XRE_USE_VALUE_USE_STRING_H
#define XRE_USE_VALUE_USE_STRING_H


#include "../context/main.h"
#include "../use_ref.h"


struct XREStateString {
    struct XRERef ref;
};


struct XREStateString * xre_use_string(struct XREContext *, char const *);
char const * xre_state_get_string(struct XREStateString *);
size_t xre_state_get_string_len(struct XREStateString *);
void xre_state_set_string(struct XREStateString *, char const *);
int xre_state_string_has_changed(struct XREStateString const *); // TODO int -> XRE_BOOL


#ifndef XRE_CONFIG_NOT_STRUCT_ALIAS
typedef struct XREStateString XREStateString;
#endif


#endif
