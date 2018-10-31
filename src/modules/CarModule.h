#ifndef INC_2019_CARMODULE_H
#define INC_2019_CARMODULE_H

#include <glm/glm.hpp>

#include "Scene.h"
#include "imgui/imgui.h"
#include "helpers/Helpers.h"

#include "modules/GuiModule.h"
#include "modules/MarkerModule.h"

class CarModule {

public:

    Model carModel{"models/spatz.obj"};

    Camera mainCamera;

    FrameBuffer frameBuffer{1, 1};
    FrameBuffer bayerFrameBuffer{1, 1, GL_RED, GL_RED};

    CarModule();

    void update(Scene::Car& car, float deltaTime);

    void render(GLuint shaderProgramId, Scene::Car& car);
};

#endif
