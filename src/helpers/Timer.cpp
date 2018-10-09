#include "Timer.h"

Timer::Timer () {

    currentTime = std::chrono::steady_clock::now();
}

void Timer::frameStep() {

    auto newTime = std::chrono::steady_clock::now();

    frameTime = std::chrono::duration_cast<std::chrono::microseconds>(
            newTime - currentTime).count() / 1000.0f;
    currentTime = newTime;

    accumulator += frameTime;
}

bool Timer::updateStep(double deltaTimeMilliSeconds) {

    if (accumulator >= deltaTimeMilliSeconds) {
        accumulator -= deltaTimeMilliSeconds;
        time += deltaTimeMilliSeconds;
        return true;
    } else {
        return false;
    }
}
