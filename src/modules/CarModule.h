#ifndef INC_2019_CARMODULE_H
#define INC_2019_CARMODULE_H

#include <glm/glm.hpp>

#include "Scene.h"
#include "helpers/Helpers.h"
#include "modules/MarkerModule.h"

class CarModule {

public:

    Model carModel{"models/spatz.obj"};

    Camera mainCamera;
    FrameBuffer frameBuffer{1, 1};

    CarModule();

    void update(Scene::Car& car, float deltaTime);

    void render(Scene::Car& car, GLuint shaderProgramId, MarkerModule& markerModule);
};

#endif
