#include "Timer.h"

Timer::Timer () {

}

void Timer::frameStep(float frameDeltaTime) {

    accumulator += frameDeltaTime;
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
