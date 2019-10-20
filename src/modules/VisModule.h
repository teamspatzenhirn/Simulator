#ifndef INC_2019_VIS_H
#define INC_2019_VIS_H

#include <deque>

#define _USE_MATH_DEFINES
#include <cmath>

#define GLM_ENABLE_EXPERIMENTAL 
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/io.hpp>

#include "helpers/Helpers.h"
#include "scene/Scene.h"
#include "scene/Settings.h"

class VisModule {

    struct StampedPose {

        double time;
        Pose pose;
    };

    double lastTraceTime;
    std::deque<StampedPose> tracedPoses;

    void drawLine(
            GLuint shaderProgramId, 
            Model& model, 
            glm::vec3 start, 
            glm::vec3 end, 
            float width, 
            glm::vec3 color);

    void drawLine(
            GLuint shaderProgramId, 
            Model& model, 
            glm::vec2 start, 
            glm::vec2 end, 
            float width, 
            glm::vec3 color);

    void drawArrow(
            GLuint shaderProgramId, 
            Model& model,
            glm::vec3 start, 
            glm::vec3 end, 
            float scale, 
            glm::vec3 color);

public:

    VisModule();

    void addPoseTrace(Pose& pose, double simulationTime);

    void drawModel(
            GLuint shaderProgramId, 
            Model& model, 
            glm::vec3 position, 
            float scale, 
            glm::vec3 color);

    void renderSensors(
            GLuint shaderProgramId,
            Model& lineModel, 
            Model& markerModel, 
            Car& car, 
            Settings& settings);

    void renderPoseTrace(
            GLuint shaderProgramId,
            Model& pointModel,
            double simulationTime,
            bool fancy);

    void renderDynamicItems(
            GLuint shaderProgramId,
            Model& lineModel,
            double simulationTime, 
            std::vector<Scene::Item>& items);

    void renderTrackPath(
            GLuint shaderProgramId,
            Model& pointModel,
            Tracks& tracks);

    void renderVisualization(
            GLuint shaderProgramId,
            Model& lineModel,
            Model& endPointModel,
            Scene::Visualization& visualization,
            Settings& settings);
};

#endif
