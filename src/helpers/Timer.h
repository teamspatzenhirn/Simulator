#ifndef INC_2019_TIMER_H
#define INC_2019_TIMER_H

#include <chrono>

class Timer {

    std::chrono::steady_clock::time_point currentTime;

public:

    float accumulator;

    Timer();

    double time;
    float frameTime;

    void frameStep();
    bool updateStep(float deltaTime);
};

#endif
