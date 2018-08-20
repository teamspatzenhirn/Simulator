#ifndef INC_2019_TIMER_H
#define INC_2019_TIMER_H

#include <chrono>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Timer {

    std::chrono::system_clock::time_point beforeRender;

public:

    std::chrono::milliseconds dt;

    void beginFrame();
    void endFrame();
};

#endif
