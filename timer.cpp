#include <sys/time.h>
#include <stdio.h>
#include <errno.h>

#include "timer.h"
#define BILLION 1000000000L

Timer::Timer() {}

void Timer::start() {
    if (clock_gettime(CLOCK_REALTIME, &startval)) {
        perror("could not get start time");
    }
}

void Timer::stop() {
    if (clock_gettime(CLOCK_REALTIME, &stopval)) {
        perror("could not get stop time");
    }
}

int Timer::getms() {
    long ns = BILLION * (stopval.tv_sec - startval.tv_sec);
    ns += (stopval.tv_nsec - startval.tv_nsec);
    return ns / 1e6;
}

int Timer::getLap() {
    struct timespec lap;
    if (clock_gettime(CLOCK_REALTIME, &lap)) {
        perror("could not get stop time");
    }

    long ns = BILLION * (lap.tv_sec - startval.tv_sec);
    ns += (lap.tv_nsec - startval.tv_nsec);
    return ns / 1e6;
}
