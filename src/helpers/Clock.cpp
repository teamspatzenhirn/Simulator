#include "Clock.h"

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
