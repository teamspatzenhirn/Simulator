#ifndef INC_2019_RULEMODULE_H
#define INC_2019_RULEMODULE_H

#include <glm/glm.hpp>

#include "Scene.h"
#include "helpers/Helpers.h"

#include "modules/CollisionModule.h"

class RuleModule {

public:

    RuleModule();

    void update(
            Scene::Rules& rules,
            Scene::Car& car,
            Scene::Tracks& tracks,
            CollisionModule& collisionModule);
};

#endif
