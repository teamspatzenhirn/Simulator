#ifndef INC_2019_TIMER_H
#define INC_2019_TIMER_H

#include <chrono>

class Timer {

public:

    Timer();

    double time;
    float accumulator;

    void frameStep(float frameDeltaTime);
    bool updateStep(float deltaTime);
};

#endif
