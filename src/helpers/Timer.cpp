#include "Timer.h"

Timer::Timer () {

    currentTime = std::chrono::system_clock::now();
}

void Timer::frameStep() {

    auto newTime = std::chrono::system_clock::now();
    frameTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            newTime - currentTime).count();
    currentTime = newTime;

    accumulator += frameTime;
}

bool Timer::updateStep(double deltaTimeMilliSeconds) {

    if (accumulator >= deltaTimeMilliSeconds) {
        accumulator -= deltaTimeMilliSeconds;
        return true;
    } else {
        return false;
    }

    time += deltaTimeMilliSeconds;
}
