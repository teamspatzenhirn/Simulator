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
    ItemType selectRandItem(std::vector<std::pair<float, ItemType>> probTable);

    bool trackIsValid(Scene& scene);

    void cleanupItems(Scene& scene);

    std::vector<ItemType> groundSpeedLimitOptions = {
        GROUND_10,
        GROUND_20,
        GROUND_30,
        GROUND_40,
        GROUND_50,
        GROUND_60,
        GROUND_70,
        GROUND_80,
        GROUND_90,
        GROUND_10_END,
        GROUND_20_END,
        GROUND_30_END,
        GROUND_40_END,
        GROUND_50_END,
        GROUND_60_END,
        GROUND_70_END,
        GROUND_80_END,
        GROUND_90_END
    };

public:

    std::deque<std::shared_ptr<ControlPoint>> controlPoints;
    std::deque<std::shared_ptr<ControlPoint>> visitedControlPoints;

    std::deque<Scene::Item*> passedItems;

    void update(Scene& scene);
};

#endif
