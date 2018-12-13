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

        uint64_t time;
        glm::vec3 position;
    };

    std::shared_ptr<Model> ringModel;
    std::shared_ptr<Model> circleModel;
    std::shared_ptr<Model> lineModel;

    uint64_t lastTraceTime;
    std::deque<StampedPosition> tracedPositions;

    void drawRing(GLuint shaderProgramId, glm::vec3 position, float scale, glm::vec3 color);
    void drawCircle(GLuint shaderProgramId, glm::vec3 position, float scale, glm::vec3 color);
    void drawLine(GLuint shaderProgramId, glm::vec2 start, glm::vec2 end, float width, glm::vec3 color);

public:

    VisModule();

    void addPositionTrace(glm::vec3 position, uint64_t simulationTime);

    void renderPositionTrace(
            GLuint shaderProgramId,
            uint64_t simulationTime,
            bool fancy);

    void renderVisualization(
            GLuint shaderProgramId,
            Scene::Visualization& visualization,
            Settings& settings);
};

#endif
