#include "./use_timespec.h"
#include "xre/impl.h"


static void assign(void *, void const *);
static int compare(struct timespec const *, struct timespec const *);


XRE_USE_X_FACTORY_IMPL_EX(
    struct timespec,
    StateTimeSpec,
    struct timespec,
    timespec,
    ((void(*)(void *))NULL),
    ((void(*)(void *, void const *)) assign),
    ((int(*)(void const *, void const *))compare)
);


inline void assign(void *dst, void const * src) {
    ((struct timespec *)dst)->tv_sec = ((struct timespec const *)src)->tv_sec;
    ((struct timespec *)dst)->tv_nsec = ((struct timespec const *)src)->tv_nsec;
};


inline int compare(struct timespec const *dst, struct timespec const * src) {
    return dst->tv_sec == src->tv_sec ? (
        dst->tv_nsec == src->tv_nsec ? 0 : 1
    ) : 1;
};
