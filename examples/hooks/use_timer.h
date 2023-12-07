#ifndef USE_TIMER_H
#define USE_TIMER_H


#include "xre.h"


struct Timer;


struct Timer * use_timer(struct XREContext *ctx);
int timer_is_done(struct Timer const *);
int timer_is_running(struct Timer const *);
double timer_get_progress(struct Timer const *);
double timer_get_time_sec(struct Timer const *);
void timer_clear(struct Timer *);
void timer_set(struct Timer *, double);


#endif
