#ifndef INC_2019_TIMER_H
#define INC_2019_TIMER_H

#include <chrono>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Timer {

    std::chrono::steady_clock::time_point currentTime;

public:

    double accumulator;

    Timer();

    double time;
    double frameTime;

    void frameStep();
    bool updateStep(double deltaTimeMilliSeconds);
};

#endif
