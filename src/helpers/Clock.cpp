#include "Clock.h"

Clock::Clock() : time{0.0}, accumulator{0.0f} {
}

void Clock::windup(float deltaTime) {

    accumulator += deltaTime;
}

bool Clock::step(float deltaTime) {

    if (accumulator >= deltaTime) {
        accumulator -= deltaTime;
        time += deltaTime;
        return true;
    } else {
        return false;
    }
}
