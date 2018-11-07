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
    Camera depthCamera;

    FrameBuffer frameBuffer{1, 1};
    FrameBuffer bayerFrameBuffer{1, 1, GL_RED, GL_RED};
    FrameBuffer depthCameraFrameBuffer{1, 1};

    CarModule();

    void updatePosition(Scene::Car& car, float deltaTime);

    void updateMainCamera(
            Scene::Car::MainCamera& carMainCamera,
            Pose& carModelPose);

    void updateDepthCamera(
            Scene::Car::DepthCamera& carDepthCamera,
            Pose& carModelPose);

    void updateLaserSensors(std::vector<std::shared_ptr<Scene::Item>>& items);

    void render(GLuint shaderProgramId, Scene::Car& car);
};

#endif
