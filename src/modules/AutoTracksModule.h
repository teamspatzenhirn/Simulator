#ifndef INC_2019_AUTOTRACKS_H
#define INC_2019_AUTOTRACKS_H

#define _USE_MATH_DEFINES
#include <cmath>
#include <deque>
#include <random>
#include <vector>

#define GLM_ENABLE_EXPERIMENTAL 
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/io.hpp>

#include "scene/Scene.h"

class AutoTracksModule {

    std::random_device randomDevice;
    std::mt19937 randomGenerator{randomDevice()};

    float rand(float min, float max);

    bool trackIsValid(Scene& scene);

public:

    std::deque<std::shared_ptr<ControlPoint>> controlPoints;
    std::deque<std::shared_ptr<ControlPoint>> visitedControlPoints;

    void update(Scene& scene);
};

#endif
