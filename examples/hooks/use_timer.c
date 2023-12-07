#include "./use_timer.h"
#include "xre.h"
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>


struct Timer {
    int is_set;
    struct timespec set_at;
    double duration_sec;
};


static void * timer_alloc(va_list);
static void timer_destroy(void *);
static void set_current_time(struct timespec *);


struct Timer * use_timer(struct XREContext *ctx) {
    struct XRERef * timer_ref = xre_use_ref(ctx, timer_alloc, timer_destroy);
    return xre_ref_get(timer_ref);
}


void timer_set(struct Timer * timer, double seconds) {
    set_current_time(&timer->set_at);

    timer->duration_sec = seconds;

    timer->is_set = 1;
};


void timer_clear(struct Timer * timer) {
    timer->is_set = 0;
};


double timer_get_time_sec(struct Timer const *timer) {
    if (timer->is_set == 0) {
        return 0.0;
    }

    struct timespec now = {0, 0};
    set_current_time(&now);

    double ndiff = now.tv_nsec - timer->set_at.tv_nsec;
    double sdiff = now.tv_sec - timer->set_at.tv_sec;
    double running_sec = ndiff / 1000000000L;
    running_sec += sdiff;

    return running_sec;
};


double timer_get_progress(struct Timer const *timer) {
    if (timer->is_set == 0) {
        return 0.0;
    }

    double running_sec = timer_get_time_sec(timer);
    if (running_sec < timer->duration_sec) {
        return running_sec / timer->duration_sec;
    }

    return 1.0;
}


inline int timer_is_done(struct Timer const *timer) {
    return timer_get_progress(timer) < 1.0 ? 0 : 1;
};


inline int timer_is_running(struct Timer const *timer) {
    if (timer->is_set == 0) {
        return 0;
    }

    return 1 - timer_is_done(timer);
};


//------------------------------------------------------------------------------
// Private
//------------------------------------------------------------------------------


void * timer_alloc(va_list args) {
    (void) args;

    struct Timer * timer = (struct Timer *) malloc(sizeof(struct Timer));

    timer->is_set = 0;
    timer->set_at = (struct timespec){0, 0};
    timer->duration_sec = 1.0;

    return timer;
};


inline void timer_destroy(void *ptr) {
    free(ptr);
};

inline void set_current_time(struct timespec *ts) {
    clock_gettime(CLOCK_MONOTONIC, ts);
}
