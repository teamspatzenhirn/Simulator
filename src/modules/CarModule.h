#ifndef INC_2019_CARMODULE_H
#define INC_2019_CARMODULE_H

#define GLM_ENABLE_EXPERIMENTAL 
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtx/intersect.hpp>

#include "scene/Car.h"
#include "scene/ModelStore.h"
#include "scene/Scene.h"

#include "helpers/Helpers.h"

#include "modules/GuiModule.h"
#include "modules/MarkerModule.h"

class CarModule {
    public:
        Camera mainCamera;
        Camera depthCamera;

        FrameBuffer frameBuffer{1, 1, 1, GL_RGBA, GL_RGBA};
        FrameBuffer bayerFrameBuffer{1, 1, 1, GL_RED, GL_RED};
        FrameBuffer depthCameraFrameBuffer{1, 1, 1, GL_RGB32F, GL_RGB};

        CarModule();

        void updatePosition(Car& car, float deltaTime);

        void updateMainCamera(
                Car::MainCamera& carMainCamera,
                Pose& carModelPose);

        void updateDepthCamera(
                Car::DepthCamera& carDepthCamera,
                Pose& carModelPose);

        void updateLaserSensors(
                Car& car,
                ModelStore& modelStore,
                std::vector<Scene::Item>& items);

        void render(GLuint shaderProgramId, Car& car, ModelStore& store);

    private:
        float calcLaserSensorValue(
                glm::vec3 position,
                glm::vec3 direction,
                ModelStore& modelStore,
                std::vector<Scene::Item>& items);

        // stores whether arrow up / down or none of both keys was pressed last
        int lastKeyPress = 0;
};

#endif
