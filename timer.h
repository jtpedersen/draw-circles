#ifndef TIMER_H_
#define TIMER_H_

#include <time.h>

class Timer {
public:
    Timer();
    void start();
    void stop();
    int getms();
    int getLap();
private:
    struct timespec startval, stopval;
};


#endif /* !TIMER_H_ */
