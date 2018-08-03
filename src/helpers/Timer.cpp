#include "Timer.h"

void Timer::beginFrame() {

    beforeRender = std::chrono::system_clock::now();    
}

void Timer::endFrame() { 

    auto afterRender = std::chrono::system_clock::now();
    dt = std::chrono::duration_cast<std::chrono::milliseconds>(
        afterRender - beforeRender);
}
