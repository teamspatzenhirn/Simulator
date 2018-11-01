#include "RuleModule.h"

RuleModule::RuleModule() {
}

float normalizeAngle(float radians) {

    while(radians > M_PI * 2) radians -= M_PI * 2;
    while(radians < 0) radians += M_PI * 2;

    return radians;
}

void RuleModule::update(
        float simulationTime,
        Scene::Rules& rules,
        Scene::Car& car,
        Scene::Tracks& tracks,
        std::vector<std::shared_ptr<Scene::Item>>& items,
        CollisionModule& collisionModule) {

    /*
     * Check if car is on the track.
     *
     * This is only a simple validation which, checks the
     * distance of the vehicle pose to the track segements
     *
     * It would be correct (according to the rules) to
     * actually check if at least three wheels are on the track.
     */
    
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
                rules.onTrack = true;
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
                    rules.onTrack = true;
                    break;
                }
            }
        }
    }

    if (!rules.onTrack) {
        if (rules.exitIfNotOnTrack) {
            std::cerr << "\nRULE VIOLATION" << std::endl;
            std::cerr << "Vehicle left track! \n" << std::endl;
            std::exit(-1);
        }
    }

    /*
     * Validating collisions
     */

    if (collisionModule.getCollisions(car.modelPose).size() > 0) {

        rules.isColliding = true;

        if (rules.exitOnObstacleCollision) {
            std::cerr << "\nRULE VIOLATION" << std::endl;
            std::cerr << "Detected collision with obstacle! \n" << std::endl;
            std::exit(-1);
        }
    }

    /*
     * Validating stop lines
     */

    for (std::shared_ptr<Scene::Item>& i : items) {
        if (i->type == STOP_LINE) {

            glm::vec3 p0 = car.modelPose.position;
            glm::vec3 p1 = Scene::getHistoryBackStep(1).car.modelPose.position;
            glm::vec3 p2 = Scene::getHistoryBackStep(2).car.modelPose.position;
            glm::vec3 p3 = Scene::getHistoryBackStep(3).car.modelPose.position;

            float d0 = glm::length(i->pose.position - p0);
            float d1 = glm::length(i->pose.position - p1);
            float d2 = glm::length(i->pose.position - p2);
            float d3 = glm::length(i->pose.position - p3);

            if (d0 > 0.3) {
                continue;
            }

            if (d3 < d2 || d1 > d0) {
                continue;
            }

            int notMovedCounter = 0;

            glm::vec3 prevPos = Scene::getFromHistory(
                    simulationTime).car.modelPose.position;

            for (int dt = 10; dt < 5000; dt += 10) {

                glm::vec3 nowPos = Scene::getFromHistory(
                        simulationTime - dt).car.modelPose.position;

                float d = glm::length(nowPos - i->pose.position);

                if (d < 0.4 && glm::length(prevPos - nowPos) < 0.001) {
                    notMovedCounter++;
                } else if (notMovedCounter > 0) {
                    break;
                }
                
                prevPos = nowPos;
            }

            if (notMovedCounter < 300) {
                std::cerr << "\nRULE VIOLATION" << std::endl;
                std::cerr << "Did not stop on stop line!" << std::endl;
                std::cerr << "Stop time: " 
                          << notMovedCounter * 10 << "ms" << std::endl;
            }
        }
    }
}


