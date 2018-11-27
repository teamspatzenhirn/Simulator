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

class VisModule {

    struct StampedPosition {

        uint64_t time;
        glm::vec3 position;
    };

    std::shared_ptr<Model> markerModel;

    uint64_t lastTraceTime;
    std::deque<StampedPosition> tracedPositions;

public:

    VisModule();

    void addPositionTrace(glm::vec3 position, uint64_t simulationTime);

    void renderPositionTrace(GLuint shaderProgramId, uint64_t simulationTime);
};

#endif
