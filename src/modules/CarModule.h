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

    void updatePosition(Scene::Car& car, float deltaTime);

    void updateMainCamera(Scene::Car& car);

    void updateLaserSensors(std::vector<std::shared_ptr<Scene::Item>>& items);

    void render(GLuint shaderProgramId, Scene::Car& car);
};

#endif
