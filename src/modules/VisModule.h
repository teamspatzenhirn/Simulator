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
#include "Scene.h"

class VisModule {

    struct StampedPosition {

        double time;
        glm::vec3 position;
    };

    double lastTraceTime;
    std::deque<StampedPosition> tracedPositions;

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

    void addPositionTrace(glm::vec3 position, double simulationTime);

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

    void renderPositionTrace(
            GLuint shaderProgramId,
            Model& pointModel,
            double simulationTime,
            bool fancy);

    void renderDynamicItems(
            GLuint shaderProgramId,
            Model& lineModel,
            double simulationTime, 
            std::vector<Scene::Item>& items);

    void renderVisualization(
            GLuint shaderProgramId,
            Model& lineModel,
            Model& endPointModel,
            Scene::Visualization& visualization,
            Settings& settings);
};

#endif
