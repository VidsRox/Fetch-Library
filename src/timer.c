#define _POSIX_C_SOURCE 199309L
#include "timer.h"

void timer_start(Timer *timer) {
    clock_gettime(CLOCK_MONOTONIC, &timer->start);
}

double timer_stop(Timer *timer) {
    clock_gettime(CLOCK_MONOTONIC, &timer->end);
    
    // Calculate difference in milliseconds
    double start_ms = timer->start.tv_sec * 1000.0 + timer->start.tv_nsec / 1000000.0;
    double end_ms = timer->end.tv_sec * 1000.0 + timer->end.tv_nsec / 1000000.0;
    
    return end_ms - start_ms;
}

double timer_now() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1000000.0;
}