#include "RuleModule.h"

RuleModule::RuleModule() {
}

void RuleModule::printViolation(double simulationTime, double drivenDistance) {

    std::cerr << "RULE VIOLATION AFTER "
              << simulationTime
              << " seconds"
              << " and "
              << drivenDistance
              << " meters!"
              << std::endl;
    std::cerr << errorMsg << std::endl;
}

bool RuleModule::update(
        double displayTime,
        double simulationTime,
        Scene::Rules& rules,
        Car& car,
        Tracks& tracks,
        std::vector<Scene::Item>& items,
        CollisionModule& collisionModule) {

    errorMsg = "";

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

    rules.onTrack = false;

    auto onLineSegment = [&carPosition](
            glm::vec2 start,
            glm::vec2 end,
            float width){

        glm::vec2 middleVec = glm::normalize(end - start);
        glm::vec2 normal(-middleVec.y, middleVec.x);

        glm::vec2 a = start + normal * width;
        glm::vec2 b = start - normal * width;
        glm::vec2 d = end - normal * width;

        glm::vec2 ab = b - a;
        glm::vec2 ad = d - a;
        glm::vec2 am = carPosition - a;

        float abDot = glm::dot(am, ab);
        float adDot = glm::dot(am, ad);

        return 0 <= abDot
                && abDot < glm::dot(ab, ab)
                && 0 < adDot
                && adDot < glm::dot(ad, ad);
    };

    for (std::shared_ptr<TrackBase>& s : trackSegments) {

        if (nullptr != dynamic_cast<TrackLine*>(s.get())) {

            TrackLine& tl = *((TrackLine*)s.get());

            glm::vec2 start = tl.start.lock()->coords;
            glm::vec2 end = tl.end.lock()->coords;

            if (onLineSegment(start, end, 0.4f)) {
                rules.onTrack = true;
                break;
            }
        }

        if (nullptr != dynamic_cast<TrackIntersection*>(s.get())) {

            TrackIntersection& ti = *((TrackIntersection*)s.get());

            glm::vec2 center = ti.center.lock()->coords;

            if (std::find_if(ti.links.begin(), ti.links.end(),
                    [&](const std::weak_ptr<ControlPoint>& link) {
                        return onLineSegment(center, link.lock()->coords, 0.4f);
                    }) != ti.links.end()) {
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

    for (Scene::Item& i : items) {

        if (TRAFFIC_ISLAND == i.type) {

            // base part of traffic island

            glm::vec4 startInModelCoords(0, 0, 1.9, 1);
            glm::vec4 endInModelCoords(0, 0, -1.9, 1);

            glm::mat4 modelMat = i.pose.getMatrix();

            glm::vec4 startInWorldCoords = modelMat * startInModelCoords;
            glm::vec4 endInWorldCoords = modelMat * endInModelCoords;

            glm::vec2 start(startInWorldCoords.x, startInWorldCoords.z);
            glm::vec2 end(endInWorldCoords.x, endInWorldCoords.z);

            if (onLineSegment(start, end, 0.4f)) {
                rules.onTrack = true;
                break;
            }

            // wider part of traffic island

            startInModelCoords = glm::vec4(0, 0, 0.4, 1);
            endInModelCoords = glm::vec4(0, 0, -0.4, 1);

            startInWorldCoords = modelMat * startInModelCoords;
            endInWorldCoords = modelMat * endInModelCoords;

            start = glm::vec2(startInWorldCoords.x, startInWorldCoords.z);
            end = glm::vec2(endInWorldCoords.x, endInWorldCoords.z);

            if (onLineSegment(start, end, 0.5f)) {
                rules.onTrack = true;
                break;
            }
        }

        if (PARK_SECTION == i.type) {

            glm::vec4 startInModelCoords(0, 0, 1.85, 1);
            glm::vec4 endInModelCoords(0, 0, -1.85, 1);

            glm::mat4 modelMat = i.pose.getMatrix();

            glm::vec4 startInWorldCoords = modelMat * startInModelCoords;
            glm::vec4 endInWorldCoords = modelMat * endInModelCoords;

            glm::vec2 start(startInWorldCoords.x, startInWorldCoords.z);
            glm::vec2 end(endInWorldCoords.x, endInWorldCoords.z);

            if (onLineSegment(start, end, 0.15f)) {
                rules.onTrack = true;
                break;
            }
        }

        if (PARK_SLOTS == i.type) {

            glm::vec4 startInModelCoords(0, 0, 1.5, 1);
            glm::vec4 endInModelCoords(0, 0, -1.5, 1);

            glm::mat4 modelMat = i.pose.getMatrix();

            glm::vec4 startInWorldCoords = modelMat * startInModelCoords;
            glm::vec4 endInWorldCoords = modelMat * endInModelCoords;

            glm::vec2 start(startInWorldCoords.x, startInWorldCoords.z);
            glm::vec2 end(endInWorldCoords.x, endInWorldCoords.z);

            if (onLineSegment(start, end, 0.25f)) {
                rules.onTrack = true;
                break;
            }
        }

        if (START_BOX == i.type) {

            glm::vec4 startInModelCoords(0, 0, 0.85, 1);
            glm::vec4 endInModelCoords(0, 0, -0.85, 1);

            glm::mat4 modelMat = i.pose.getMatrix();

            glm::vec4 startInWorldCoords = modelMat * startInModelCoords;
            glm::vec4 endInWorldCoords = modelMat * endInModelCoords;

            glm::vec2 start(startInWorldCoords.x, startInWorldCoords.z);
            glm::vec2 end(endInWorldCoords.x, endInWorldCoords.z);

            if (onLineSegment(start, end, 0.19f)) {
                rules.onTrack = true;
                break;
            }
        }
    }

    /*
     * Validating collisions
     */

    rules.isColliding = collisionModule.getCollisions(
            car.modelPose).size() > 0;

    /*
     * Validating items
     */

    bool allCheckpointsPassed = true;
    int checkpointCounter = 0;

    for (Scene::Item& i : items) {

        float d = glm::length(i.pose.position - car.modelPose.position);
        bool isReallyClose = d < 0.15;

        switch (i.type) {
            case CROSSWALK:
            case CROSSWALK_SMALL:
            case STOP_LINE:
            case GIVE_WAY_LINE:
                if (CROSSWALK == i.type) {
                    isReallyClose = d < 0.3;
                }
                if (!rules.lineId) {
                    if (d < 0.5) {
                        rules.lineId = i.id;
                        rules.lineTime = simulationTime;
                        rules.linePassed = false;
                    }
                } else if (rules.lineId == i.id) {
                    if (isReallyClose && rules.linePassed == false) {
                        double delta = simulationTime - rules.lineTime;
                        double deltaLimit = 0;
                        std::string typeString = "";
                        if (i.type == STOP_LINE) {
                            deltaLimit = 3.0;
                            typeString = "stop line";
                        }
                        if (i.type == GIVE_WAY_LINE) {
                            deltaLimit = 1.0;
                            typeString = "give-way line";
                        }
                        if (i.type == CROSSWALK || i.type == CROSSWALK_SMALL) {
                            bool pedestrianNearby = false;
                            for (Scene::Item& j : items) {
                                if((j.type == DYNAMIC_PEDESTRIAN_LEFT
                                        || j.type == DYNAMIC_PEDESTRIAN_RIGHT)
                                        && glm::length(j.pose.position - i.pose.position) < 1) {
                                   pedestrianNearby = true;
                                   break;
                                }
                            }
                            if (pedestrianNearby) {
                                deltaLimit = 1.000;
                            } else {
                                deltaLimit = 0;
                            }
                            typeString = "crosswalk";
                        }
                        if (delta < deltaLimit) {

                            if (i.type == GIVE_WAY_LINE) {
                                rules.giveWayLineIgnored = true;
                                errorMsg = "Passed "
                                      + typeString
                                      + " in: "
                                      + std::to_string(delta)
                                      + "s";
                            }
                            if (i.type == STOP_LINE) {
                                rules.stopLineIgnored = true;
                                errorMsg = "Passed "
                                    + typeString
                                    + " in: "
                                    + std::to_string(delta)
                                    + "s";
                            }
                            if (i.type == CROSSWALK) {
                                rules.crosswalkIgnored = true;
                                errorMsg = "Passed "
                                    + typeString
                                    + " in: "
                                    + std::to_string(delta)
                                    + "s";
                            }
                        }
                        rules.linePassed = true;
                    }
                    if (d > 0.5) {
                        rules.lineId = 0;
                        rules.lineTime = 0;
                        rules.linePassed = false;

                        rules.stopLineIgnored = false;
                        rules.giveWayLineIgnored = false;
                        rules.crosswalkIgnored = false;
                    }
                }
                break;

            case GROUND_10:
                if (isReallyClose) rules.allowedMaxSpeed = 10;
                break;
            case GROUND_20:
                if (isReallyClose) rules.allowedMaxSpeed = 20;
                break;
            case GROUND_30:
                if (isReallyClose) rules.allowedMaxSpeed = 30;
                break;
            case GROUND_40:
                if (isReallyClose) rules.allowedMaxSpeed = 40;
                break;
            case GROUND_50:
                if (isReallyClose) rules.allowedMaxSpeed = 50;
                break;
            case GROUND_60:
                if (isReallyClose) rules.allowedMaxSpeed = 60;
                break;
            case GROUND_70:
                if (isReallyClose) rules.allowedMaxSpeed = 70;
                break;
            case GROUND_80:
                if (isReallyClose) rules.allowedMaxSpeed = 80;
                break;
            case GROUND_90:
                if (isReallyClose) rules.allowedMaxSpeed = 90;
                break;

            case GROUND_10_END:
            case GROUND_20_END:
            case GROUND_30_END:
            case GROUND_40_END:
            case GROUND_50_END:
            case GROUND_60_END:
            case GROUND_70_END:
            case GROUND_80_END:
            case GROUND_90_END:
                if (isReallyClose) rules.allowedMaxSpeed = 1000;
                break;

            case GROUND_ARROW_RIGHT:
                if (!rules.rightArrowId && isReallyClose) {
                    rules.rightArrowId = i.id;
                }
                if (rules.rightArrowId == i.id) {
                    glm::vec4 carWorldCoords =
                        glm::vec4(car.modelPose.position, 1.0f);
                    glm::vec3 carArrowCoords = glm::vec3(
                            i.pose.getInverseMatrix() * carWorldCoords);

                    rules.rightArrowIgnored = carArrowCoords.x < -0.5
                            || carArrowCoords.z < -1.5
                            || carArrowCoords.z > 0.3;

                    if (carArrowCoords.x > 0.5
                            || glm::length(carArrowCoords) > 3) {
                        rules.rightArrowId = 0;
                    }
                }
                break;
            case GROUND_ARROW_LEFT:
                if (!rules.leftArrowId && isReallyClose) {
                    rules.leftArrowId = i.id;
                }
                if (rules.leftArrowId == i.id) {

                    glm::vec4 carWorldCoords =
                        glm::vec4(car.modelPose.position, 1.0f);
                    glm::vec3 carArrowCoords = glm::vec3(
                            i.pose.getInverseMatrix() * carWorldCoords);

                    rules.leftArrowIgnored = carArrowCoords.x > 0.2
                            || carArrowCoords.z < -1.5
                            || carArrowCoords.z > 0.3;

                    if (carArrowCoords.x < -0.9
                            || glm::length(carArrowCoords) > 3) {
                        rules.leftArrowId = 0;
                    }
                }
                break;

            case NO_PARKING:
                rules.noParkingIgnored = isReallyClose;
                break;

            case CHECKPOINT:
                if (std::find(
                            rules.passedCheckpointIds.begin(),
                            rules.passedCheckpointIds.end(),
                            i.id) == rules.passedCheckpointIds.end()) {
                    allCheckpointsPassed = false;
                    if (isReallyClose) {
                        rules.passedCheckpointIds.push_back(i.id);
                    }
                }
                checkpointCounter += 1;
                break;

            default:
                break;
        }
    }

    /*
     * Validate speed limits
     */

    const double tolerance = 0.1;

    rules.speedLimitExceeded = car.vesc.velocity - rules.allowedMaxSpeed
                / 3.6 / 10.0 > tolerance;

    /*
     * Validate lack of progress
     */
    
    if ((car.drivenDistance - rules.lastDrivenDistance) > 0) {
        rules.lastIteractionTime = displayTime;
    }
    rules.lastDrivenDistance = car.drivenDistance;
    rules.lackOfProgress = displayTime - rules.lastIteractionTime > 20;

    /*
     * Generate error messages and exit (if necessary)
     */

    if (rules.lackOfProgress) {
        errorMsg = "Lack of progress!";
    }
    if (rules.speedLimitExceeded) {
        errorMsg = "Speed limit of "
              + std::to_string(rules.allowedMaxSpeed / 3.6 / 10)
              + " but car speed is "
              + std::to_string(car.vesc.velocity);
        if (rules.exitIfSpeedLimitExceeded) {
            printViolation(simulationTime, car.drivenDistance);
            std::exit(-1);
        }
    }
    if (rules.noParkingIgnored) {
        errorMsg = "Ignored no parking!";
        if (rules.exitIfNoParkingIgnored) {
            printViolation(simulationTime, car.drivenDistance);
            std::exit(-1);
        }
    }
    if (rules.leftArrowIgnored) {
        errorMsg = "Ignored left arrow!";
        if (rules.exitIfLeftArrowIgnored) {
            printViolation(simulationTime, car.drivenDistance);
            std::exit(-1);
        }
    }
    if (rules.rightArrowIgnored) {
        errorMsg = "Ignored right arrow!";
        if (rules.exitIfRightArrowIgnored) {
            printViolation(simulationTime, car.drivenDistance);
            std::exit(-1);
        }
    }
    if (rules.isColliding) {
        errorMsg = "Detected collision with obstacle!";
        if (rules.exitOnObstacleCollision) {
            printViolation(simulationTime, car.drivenDistance);
            std::exit(-1);
        }
    }
    if (!rules.onTrack) {
        errorMsg = "Vehicle left track!";
        if (rules.exitIfNotOnTrack) {
            printViolation(simulationTime, car.drivenDistance);
            std::exit(-1);
        }
    }
    if (rules.crosswalkIgnored) { 
        if (rules.exitIfCrosswalkIgnored) {
            printViolation(simulationTime, car.drivenDistance);
            std::exit(-1);
        }
    }
    if (rules.stopLineIgnored) { 
        if (rules.exitIfStopLineIgnored) {
            printViolation(simulationTime, car.drivenDistance);
            std::exit(-1);
        }
    }
    if (rules.giveWayLineIgnored) { 
        if (rules.exitIfGiveWayLineIgnored) {
            printViolation(simulationTime, car.drivenDistance);
            std::exit(-1);
        }
    }

    /*
     * Exit normally if all checkpoints were passed
     */

    if (allCheckpointsPassed
            && checkpointCounter != 0
            && rules.exitIfAllCheckpointsPassed) {
        std::exit(0);
    }

    return errorMsg.empty();
}
