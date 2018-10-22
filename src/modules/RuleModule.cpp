#include "RuleModule.h"

RuleModule::RuleModule() {
}

void RuleModule::update(
        Scene::Rules& rules,
        Scene::Car& car,
        CollisionModule& collisionModule) {

    if (collisionModule.getCollisions(car.modelPose).size() > 0) {
        if (rules.exitOnObstacleCollision) {

            std::cerr << "\nRULE VIOLATION" << std::endl;
            std::cerr << "Detected collision with obstacle! \n" << std::endl;
            std::exit(-1);
        }
    }
}
