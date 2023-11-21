#include "./use_timespec.h"


static void assign(struct timespec *, struct timespec const *);


XRE_USE_X_FACTORY_EX(
    struct timespec,
    StateTimeSpec,
    struct timespec,
    timespec,
    assign
);


inline void assign(struct timespec *dst, struct timespec const * src) {
    dst->tv_sec = src->tv_sec;
    dst->tv_nsec = src->tv_nsec;
};
