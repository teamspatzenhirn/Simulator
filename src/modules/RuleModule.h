#ifndef INC_2019_RULEMODULE_H
#define INC_2019_RULEMODULE_H

#include <vector>
#include <algorithm>

#include <glm/glm.hpp>

#include "Scene.h"
#include "helpers/Helpers.h"

#include "modules/CollisionModule.h"

class RuleModule {

    void printViolation(float simulationTime);

public:

    RuleModule();

    void update(
            float simulationTime,
            Scene::Rules& rules,
            Scene::Car& car,
            Scene::Tracks& tracks,
            std::vector<std::shared_ptr<Scene::Item>>& items,
            CollisionModule& collisionModule);
};

#endif
