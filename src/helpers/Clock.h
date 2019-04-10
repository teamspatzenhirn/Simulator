#ifndef INC_2019_CLOCK_H
#define INC_2019_CLOCK_H

#include <chrono>

class Clock {

public:

    double time;
    float accumulator;

    void windup(float deltaTime);
    bool step(float deltaTime);
};

#endif
