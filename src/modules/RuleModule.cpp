#include "RuleModule.h"

RuleModule::RuleModule() {
}

float normalizeAngle(float radians) {

    while(radians > M_PI * 2) radians -= M_PI * 2;
    while(radians < 0) radians += M_PI * 2;

    return radians;
}

void RuleModule::update(
        Scene::Rules& rules,
        Scene::Car& car,
        Scene::Tracks& tracks,
        CollisionModule& collisionModule) {

    // checking that car is on the right side of the track
    
    // this is only a simple validation which checks the
    // distance of the vehicle pose to the track segements

    // it would be correct (according to the rules) to
    // actually check if at least three wheels are on the track
    
    std::vector<std::shared_ptr<TrackBase>> trackSegments;

    for (const std::shared_ptr<ControlPoint>& p : tracks.getTracks()) {
        for (std::shared_ptr<TrackBase>& t : p->tracks) {
            if (trackSegments.end() == 
                    std::find(trackSegments.begin(), trackSegments.end(), t)) {
                trackSegments.push_back(t);
            }
        }
    }
    
    glm::vec2 carPosition(
            car.modelPose.position.x, 
            car.modelPose.position.z);

    bool onTrack = false;

    for (std::shared_ptr<TrackBase>& s : trackSegments) {

        if (nullptr != dynamic_cast<TrackLine*>(s.get())) {

            TrackLine& tl = *((TrackLine*)s.get());

            glm::vec2 start = tl.start.lock()->coords;
            glm::vec2 end = tl.end.lock()->coords;

            glm::vec2 middleVec = glm::normalize(end - start);
            glm::vec2 normal(-middleVec.y, middleVec.x);

            glm::vec2 a = start + normal * 0.4f;
            glm::vec2 b = start - normal * 0.4f;
            glm::vec2 d = end - normal * 0.4f;

            glm::vec2 ab = b - a;
            glm::vec2 ad = d - a;
            glm::vec2 am = carPosition - a;

            float abDot = glm::dot(am, ab);
            float adDot = glm::dot(am, ad);

            if (0 <= abDot
                    && abDot < glm::dot(ab, ab)
                    && 0 < adDot
                    && adDot < glm::dot(ad, ad)) {
                onTrack = true;
                break;
            }
        }

        if (nullptr != dynamic_cast<TrackArc*>(s.get())) {

            TrackArc& ta = *((TrackArc*)s.get());

            glm::vec2 start = ta.start.lock()->coords;
            glm::vec2 end = ta.end.lock()->coords;

            glm::vec2 startVec = start - ta.center;
            glm::vec2 endVec = end - ta.center;
            glm::vec2 carVec = carPosition - ta.center;

            bool inSection = false;

            glm::vec2 startVecNormal(-startVec.y, startVec.x);
            glm::vec2 endVecNormal(-endVec.y, endVec.x);

            if (ta.rightArc) {
                if (glm::dot(endVecNormal, carVec) < 0
                        && glm::dot(startVecNormal, carVec) > 0) {
                    inSection = true;
                }
            } else {
                if (glm::dot(startVecNormal, carVec) < 0
                        && glm::dot(endVecNormal, carVec) > 0) {
                    inSection = true;
                }
            }

            if (inSection) { 
                float dist = glm::length(carVec);
                if (dist > ta.radius - 0.4 && dist < ta.radius + 0.4) {
                    onTrack = true;
                    break;
                }
            }
        }
    }

    if (!onTrack) {
        if (rules.exitIfNotOnTrack) {
            std::cerr << "\nRULE VIOLATION" << std::endl;
            std::cerr << "Vehicle left track! \n" << std::endl;
            std::exit(-1);
        }
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


