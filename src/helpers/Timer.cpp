#include "Timer.h"

Timer::Timer () {

    currentTime = std::chrono::steady_clock::now();
}

void Timer::frameStep() {

    auto newTime = std::chrono::steady_clock::now();

    frameTime = (float)std::chrono::duration_cast<std::chrono::microseconds>(
            newTime - currentTime).count() / 1000000.0f;
    currentTime = newTime;

    accumulator += frameTime;
}

bool Timer::updateStep(float deltaTime) {

    if (accumulator >= deltaTime) {
        accumulator -= deltaTime;
        time += deltaTime;
        return true;
    } else {
        return false;
    }
}
