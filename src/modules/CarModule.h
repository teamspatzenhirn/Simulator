#ifndef INC_2019_CARMODULE_H
#define INC_2019_CARMODULE_H

#include <glm/glm.hpp>

#include "helpers/Helpers.h"

#include "Scene.h"

class CarModule {

public:

    CarModule();

    Camera mainCamera;
    FrameBuffer frameBuffer{1, 1};

    void update(Scene::Car& car);
};

#endif
