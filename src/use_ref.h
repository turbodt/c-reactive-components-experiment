#ifndef XRE_USE_REF_H
#define XRE_USE_REF_H


#include "./context/main.h"


struct XRERef;


typedef void * (*XRERefConstructor)(va_list);
typedef void (*XRERefDestructor)(void *);
typedef void (*XRERefAssignator)(void *, void const *);
typedef int (*XRERefComparator)(void const *, void const *); // TODO int -> XRE_BOOL


struct XRERef {
    struct XREContextState * state;
    XRERefDestructor destructor;
    XRERefAssignator assignator;
    XRERefComparator comparator;
    void * value;
    int has_changed; // TODO int -> XRE_BOOL
};


struct XRERef * xre_use_vref(
    struct XREContext *,
    XRERefConstructor,
    XRERefDestructor,
    XRERefAssignator,
    XRERefComparator,
    va_list
);


struct XRERef * xre_use_ref(
    struct XREContext *,
    XRERefConstructor,
    XRERefDestructor,
    ...
);


struct XRERef * xre_use_ref_ex(
    struct XREContext *,
    XRERefConstructor,
    XRERefDestructor,
    XRERefAssignator,
    XRERefComparator,
    ...
);


void const * xre_ref_get_const(struct XRERef const *);
void * xre_ref_get(struct XRERef *);
void xre_ref_set(struct XRERef *, void *);
int xre_ref_cmp(struct XRERef const *, void const *); // TODO int -> XRE_BOOL
int xre_ref_has_changed(struct XRERef const *); // TODO int -> XRE_BOOL


#endif
