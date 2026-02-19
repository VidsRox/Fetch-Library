#ifndef TIMER_H
#define TIMER_H

#include <time.h>

typedef struct {
    struct timespec start;
    struct timespec end;
} Timer;

// Start the timer
void timer_start(Timer *timer);

// Stop the timer and return elapsed time in milliseconds
double timer_stop(Timer *timer);

// Get current timestamp (for individual request timing)
double timer_now();

#endif // TIMER_H