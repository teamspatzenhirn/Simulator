#include "modules/AutoTracksModule.h"

float AutoTracksModule::rand(float min, float max) { 

    std::uniform_real_distribution<float> distribution(min, max);
    return distribution(randomGenerator);
}

bool AutoTracksModule::trackIsValid(Scene& scene) {

    std::vector<glm::vec2> trackPoints = scene.tracks.getPath(0.1f);

    for (size_t i = 0; i < trackPoints.size(); ++i) {
        for (size_t j = 0; j < trackPoints.size(); ++j) {

            glm::vec2 currentPoint = trackPoints[i];
            glm::vec2 otherPoint = trackPoints[j];

            if (glm::length(currentPoint - otherPoint) < 1.0
                    && std::abs((int)(i - j)) > 12) {
                return false;
            }
        }
    }

    return true;
}

void AutoTracksModule::update(Scene& scene) {

    std::vector<std::shared_ptr<ControlPoint>> tracks
        = scene.tracks.getTracks();

    if (tracks.size() > 0) {
        std::shared_ptr<ControlPoint> c = controlPoints.front();
        glm::vec2 carPosition = glm::vec2(
                    scene.car.modelPose.position.x,
                    scene.car.modelPose.position.z);
        if (glm::length(c->coords - carPosition) < 0.3) {
            if (visitedControlPoints.end() == std::find(
                        visitedControlPoints.begin(),
                        visitedControlPoints.end(), c)) {
                visitedControlPoints.push_back(c);
                controlPoints.pop_front();
            }
        }
    }

    while (visitedControlPoints.size() > 3) {
        scene.tracks.removeControlPoint(visitedControlPoints.front());
        visitedControlPoints.pop_front();
    }

    unsigned int failCounter = 0;

    while (tracks.size() < 10) {

        tracks = scene.tracks.getTracks();

        if (tracks.size() == 0) {

            // generate straight track segment as start 

            auto start = std::make_shared<ControlPoint>();
            start->coords = glm::vec2(
                    scene.car.modelPose.position.x + 0.2,
                    scene.car.modelPose.position.z - 0.2);
            controlPoints.push_back(start);

            auto end = std::make_shared<ControlPoint>();
            end->coords = glm::vec2(
                    scene.car.modelPose.position.x + 0.2,
                    scene.car.modelPose.position.z + rand(0.5, 3.0));
            controlPoints.push_back(end);

            scene.tracks.addTrackLine(start, end);
        } else {
            auto last = controlPoints.back();
            std::shared_ptr<TrackBase> trackSegment = last->tracks.front();
            glm::vec2 dir = glm::normalize(trackSegment->getDirection(*last));
            glm::vec2 ortho = glm::vec2(-dir.y, dir.x);

            auto end = std::make_shared<ControlPoint>();

            if (rand(0.0, 1.0) < 0.8) { 
                float radius = rand(0.8, 3.0);
                bool rightArc = rand(0.0, 1.0) > 0.5;
                float arcLength = rand(0.0, M_PI);

                // TODO: 

                glm::vec2 centerCoords = last->coords
                        + (rightArc ? 1.0f : -1.0f) * ortho * radius;

                glm::vec2 localEndCoords(
                        std::cos(arcLength) * radius,
                        std::sin(arcLength) * radius);

                glm::mat2 toGlobalMat = glm::inverse(glm::mat2(ortho, dir));
                glm::vec2 endCoords = centerCoords + toGlobalMat * localEndCoords;
                        
                end->coords = endCoords;

                if (glm::length(end->coords) > 20) {
                    if (failCounter > 25 && tracks.size() > 3) {
                        scene.tracks.removeControlPoint(controlPoints.back());
                        controlPoints.pop_back();
                        failCounter = 0;
                    } else {
                        failCounter += 1;
                    }
                } else {
                    scene.tracks.addTrackArc(
                            last, end, centerCoords, radius, rightArc);
                    controlPoints.push_back(end);
                }
            } else { 
                float length = rand(0.5, 2.0);
                end->coords = last->coords + dir * length;

                if (glm::length(end->coords) > 20) {
                    if (failCounter > 25 && tracks.size() > 3) {
                        scene.tracks.removeControlPoint(controlPoints.back());
                        controlPoints.pop_back();
                        failCounter = 0;
                    } else {
                        failCounter += 1;
                    }
                } else {
                    scene.tracks.addTrackLine(last, end);
                    controlPoints.push_back(end);
                }
            }

            if (!trackIsValid(scene)) {
                scene.tracks.removeControlPoint(controlPoints.back());
                controlPoints.pop_back();

                if (failCounter > 25 && tracks.size() > 3) {
                    scene.tracks.removeControlPoint(controlPoints.back());
                    controlPoints.pop_back();
                    failCounter = 0;
                } else {
                    failCounter += 1;
                }
            }
        }
    }
}
