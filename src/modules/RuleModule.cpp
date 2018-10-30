#include "RuleModule.h"

RuleModule::RuleModule() {
}

void RuleModule::update(
        Scene::Rules& rules,
        Scene::Car& car,
        Scene::Tracks& tracks,
        CollisionModule& collisionModule) {

    // checking that car is on the right side of the track

    for (const std::shared_ptr<ControlPoint>& p : tracks.getTracks()) {
    }

    // validating collisions

    if (collisionModule.getCollisions(car.modelPose).size() > 0) {
        if (rules.exitOnObstacleCollision) {
            std::cerr << "\nRULE VIOLATION" << std::endl;
            std::cerr << "Detected collision with obstacle! \n" << std::endl;
            std::exit(-1);
        }
    }
}


