#ifndef XRE_USE_VALUE_USE_STRING_H
#define XRE_USE_VALUE_USE_STRING_H


#include "../context/main.h"
#include "../use_ref.h"


struct XREStateString {
    struct XRERef * use_ref;
};


struct XREStateString * xre_use_string(struct XREContext *, char const *);
char const * xre_state_get_string(struct XREStateString *);
void xre_state_set_string(struct XREStateString *, char const *);
int xre_state_string_has_changed(struct XREStateString const *); // TODO int -> XRE_BOOL


#endif
