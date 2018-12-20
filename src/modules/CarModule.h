#ifndef INC_2019_CARMODULE_H
#define INC_2019_CARMODULE_H

#define GLM_ENABLE_EXPERIMENTAL 
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtx/intersect.hpp>

#include "Scene.h"
#include "imgui/imgui.h"
#include "helpers/Helpers.h"

#include "modules/GuiModule.h"
#include "modules/MarkerModule.h"

class CarModule {

    float calcLaserSensorValue(
            glm::vec3 position,
            glm::vec3 direction,
            ModelStore& modelStore,
            std::vector<std::shared_ptr<Scene::Item>>& items);

public:

    Model carModel{"models/spatz.obj"};

    Camera mainCamera;
    Camera depthCamera;

    FrameBuffer frameBuffer{1, 1};
    FrameBuffer bayerFrameBuffer{1, 1, GL_RED, GL_RED};
    FrameBuffer depthCameraFrameBuffer{1, 1, GL_RGB32F, GL_RGB};

    CarModule();

    void updatePosition(Scene::Car& car, float deltaTime);

    void updateMainCamera(
            Scene::Car::MainCamera& carMainCamera,
            Pose& carModelPose);

    void updateDepthCamera(
            Scene::Car::DepthCamera& carDepthCamera,
            Pose& carModelPose);

    void updateLaserSensors(
            Scene::Car& car,
            ModelStore& modelStore,
            std::vector<std::shared_ptr<Scene::Item>>& items);

    void render(GLuint shaderProgramId, Scene::Car& car);
};

#endif
