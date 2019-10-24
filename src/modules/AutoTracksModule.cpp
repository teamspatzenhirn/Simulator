#include <algorithm>

#include "modules/AutoTracksModule.h"

float AutoTracksModule::rand(float min, float max) { 

    std::uniform_real_distribution<float> distribution(min, max);
    return distribution(randomGenerator);
}

ItemType AutoTracksModule::selectRandItem(std::vector<std::pair<float, ItemType>> probTable) {

    float r = rand(0.0, 1.0);
    float lowerMargin = 0.0f;

    for (std::pair<float, ItemType>& p: probTable) {
        if (r >= lowerMargin && r < lowerMargin + std::get<0>(p)) {
            return std::get<1>(p);
        }
        lowerMargin += std::get<0>(p);
    }

    return ItemType::NONE;
}

void AutoTracksModule::cleanupItems(Scene& scene) { 

    std::vector<glm::vec2> trackPoints = scene.tracks.getPath(0.1);
    
    scene.items.erase(
            std::remove_if(
                scene.items.begin(),
                scene.items.end(),
                [&](const Scene::Item& item) {
                    float minDist = 100000.0f;
                    for (glm::vec2& point : trackPoints) {
                        float d = glm::length(item.pose.position 
                                - glm::vec3(point.x, 0.0f, point.y));
                        minDist = std::min(d, minDist);
                    }
                    return minDist > 0.6f;
                }),
            scene.items.end());
}

bool AutoTracksModule::trackIsValid(Scene& scene) {

    std::vector<glm::vec2> trackPoints = scene.tracks.getPath(0.1f);

    for (size_t i = 0; i < trackPoints.size(); ++i) {
        for (size_t j = 0; j < trackPoints.size(); ++j) {

            glm::vec2 currentPoint = trackPoints[i];
            glm::vec2 otherPoint = trackPoints[j];

            if (glm::length(currentPoint - otherPoint) < 2.0
                    && std::abs((int)(i - j)) > 24) {
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

    std::vector<std::shared_ptr<TrackBase>> trackSegments
        = scene.tracks.getTrackSegments();

    while (trackSegments.size() < 10) {

        tracks = scene.tracks.getTracks();
        trackSegments = scene.tracks.getTrackSegments();

        if (tracks.size() == 0) {

            // generate straight track segment as start 

            auto start = std::make_shared<ControlPoint>();
            start->coords = glm::vec2(
                    scene.car.modelPose.position.x + scene.tracks.laneWidth / 2,
                    scene.car.modelPose.position.z - scene.tracks.laneWidth / 2);
            controlPoints.push_back(start);

            auto end = std::make_shared<ControlPoint>();
            end->coords = glm::vec2(
                    scene.car.modelPose.position.x + scene.tracks.laneWidth / 2,
                    scene.car.modelPose.position.z + rand(0.5, 3.0));
            controlPoints.push_back(end);

            scene.tracks.addTrackLine(start, end);
        } else {

            // create some helpful variables

            const auto last = controlPoints.back();
            const glm::vec2 dir = glm::normalize(last->tracks.front()->getDirection(*last));
            const glm::vec2 ortho = glm::vec2(-dir.y, dir.x);

            auto end = std::make_shared<ControlPoint>();

            std::shared_ptr<TrackBase> genTrack = nullptr;

            // generate either a straight, arc, or intersection 
            
            float rtype = rand(0.0, 1.0);
            if (rtype > 0.5) { 
                // generate arc
                
                float radius = rand(0.8, 3.0);
                bool rightArc = rand(0.0, 1.0) > 0.5;
                float arcLength = rand(0.0, M_PI);

                glm::vec2 centerCoords = last->coords
                        + (rightArc ? 1.0f : -1.0f) * ortho * radius;

                glm::vec2 localEndCoords(
                        std::cos(arcLength) * radius,
                        std::sin(arcLength) * radius);

                glm::mat2 toGlobalMat = glm::inverse(glm::mat2(ortho, dir));
                glm::vec2 endCoords = centerCoords + toGlobalMat * localEndCoords;
                        
                end->coords = endCoords;

                genTrack = scene.tracks.addTrackArc(
                        last, end, centerCoords, radius, rightArc);
                controlPoints.push_back(end);

            } else if (rtype > 0.3) { 
                // generate crossing
                
                float d = 1.5 + scene.tracks.trackWidth / 2;

                std::shared_ptr<ControlPoint> center = std::make_shared<ControlPoint>();
                center->coords = last->coords + dir * d;

                std::shared_ptr<ControlPoint> link2 = std::make_shared<ControlPoint>();
                link2->coords = last->coords + ortho * d + dir * d;
                std::shared_ptr<ControlPoint> link3 = std::make_shared<ControlPoint>();
                link3->coords = last->coords + dir * d * 2.0f;
                std::shared_ptr<ControlPoint> link4 = std::make_shared<ControlPoint>();
                link4->coords = last->coords - ortho * d + dir * d;

                genTrack = scene.tracks.addTrackIntersection(
                        center, last, link2, link3, link4);

                const float dirProb = rand(0.0, 1.0);

                end = link3;

                if (dirProb < 0.3) {
                    end = link2;
                } else if (dirProb < 0.6) {
                    end = link4;
                }

                controlPoints.push_back(end);

            } else {
                // generate straight
                
                float length = rand(0.5, 2.0);
                end->coords = last->coords + dir * length;

                genTrack = scene.tracks.addTrackLine(last, end);
                controlPoints.push_back(end);
            }

            // check if the track + generated segment is still valid 
            // else delete just created segment and try again

            if (glm::length(end->coords) > 20 || !trackIsValid(scene)) {

                scene.tracks.removeControlPoint(controlPoints.back());
                controlPoints.pop_back();

                if (failCounter > 25 && tracks.size() > 3) {
                    scene.tracks.removeControlPoint(controlPoints.back());
                    controlPoints.pop_back();
                    failCounter = 0;
                } else {
                    failCounter += 1;
                }

                cleanupItems(scene); 
                continue;
            }

            // check what track was actually generated
                
            bool createdLine = nullptr != std::dynamic_pointer_cast<TrackLine>(genTrack);
            bool createdArc = nullptr != std::dynamic_pointer_cast<TrackArc>(genTrack);

            // start placing obstacles
            
            std::vector<glm::vec2> genTrackPoints = genTrack->getPoints(0.1);

            if (genTrackPoints.size() > 2 && controlPoints.size() > 2) {

                float ps = controlPoints.size();
                float distEnd = glm::length(
                        controlPoints.at(ps-1)->coords - genTrackPoints.at(0));
                float distStart = glm::length(
                        controlPoints.at(ps-2)->coords - genTrackPoints.at(0));

                if (distEnd < distStart) {
                    std::reverse(genTrackPoints.begin(), genTrackPoints.end());
                }
            }

            if (createdLine || createdArc) {
                for (size_t i = 1; i < genTrackPoints.size(); ++i) {

                    const glm::vec2 prev = genTrackPoints[i-1];
                    glm::vec2 p = genTrackPoints[i];

                    const glm::vec2 dir = glm::normalize(prev - p);
                    const glm::vec2 ortho = glm::vec2(-dir.y, dir.x);
                    p -= ortho * 0.2f;

                    // check if too close to other items

                    bool tooClose = false;
                    for (Scene::Item& item : scene.items) {
                        if (glm::length(item.pose.position 
                                    - glm::vec3(p.x, 0.0f, p.y)) < 0.5) {
                            tooClose = true;
                            break;
                        }
                        if (tooClose) {
                            break;
                        }
                    }
                    if (glm::length(p) < 2.0) { 
                        tooClose = true;
                    }
                    if (tooClose) {
                        continue;
                    }

                    // ok, not too close ...
                    ItemType newItemType = selectRandItem({
                                {0.02, ItemType::MISSING_SPOT},
                                {0.02, ItemType::OBSTACLE},
                                {0.01, ItemType::DYNAMIC_OBSTACLE},
                                {0.02, groundSpeedLimitOptions.at(rand(0.0, 18.0))}
                            });

                    if (ItemType::NONE != newItemType) {
                        Scene::Item& newItem = scene.items.emplace_back();
                        newItem.type = newItemType;

                        if (ItemType::MISSING_SPOT == newItem.type) {
                            newItem.name = "autotrack_missing_spot";
                            newItem.pose.position = glm::vec3(
                                    p.x + rand(-0.4, 0.4),
                                    0.006f,
                                    p.y + rand(-0.4, 0.4)
                                );
                            newItem.pose.scale = glm::vec3(
                                    0.2f + rand(0.0, 0.4),
                                    1.0f,
                                    0.2f + rand(0.0, 0.4)
                                );
                            newItem.pose.setEulerAngles(
                                    {0.0, rand(0.0, 360.0), 0.0});
                        } else if (ItemType::OBSTACLE == newItem.type) {
                            newItem.name = "autotrack_obstacle";
                            newItem.pose.position = glm::vec3(
                                    p.x + rand(-0.1, 0.1),
                                    0.0f,
                                    p.y + rand(-0.1, 0.1)
                                );
                            newItem.pose.scale = glm::vec3(
                                    1.1f + rand(0.0, 2.0),
                                    1.1f + rand(0.0, 2.0), 
                                    1.1f + rand(0.0, 2.0)
                                );
                            newItem.pose.setEulerAngles(
                                    {0.0, rand(0.0, 360.0), 0.0});
                        } else if (ItemType::DYNAMIC_OBSTACLE == newItem.type) {
                            if (createdLine) {
                                newItem.name = "autotrack_dynamic_item";
                                newItem.pose.position = glm::vec3(
                                        p.x + rand(-0.05, 0.05),
                                        0.0f,
                                        p.y + rand(-0.05, 0.05)
                                    );
                                newItem.pose.scale = glm::vec3(
                                        1.0f + rand(-0.05, 0.05),
                                        1.0f + rand(-0.05, 0.05), 
                                        1.0f + rand(-0.05, 0.05)
                                    );
                                newItem.pose.setEulerAngles(
                                        {0.0, glm::degrees(std::atan2(dir.x, dir.y)) + 180, 0.0});
                            } else {
                                scene.items.pop_back();
                            }
                        } else {
                            newItem.name = "autotrack_speedlimit";
                            newItem.pose.position = glm::vec3(
                                    p.x + rand(-0.05, 0.05),
                                    0.0f,
                                    p.y + rand(-0.05, 0.05)
                                );
                            newItem.pose.scale = glm::vec3(
                                    1.0f + rand(-0.05, 0.05),
                                    1.0f + rand(-0.05, 0.05), 
                                    1.0f + rand(-0.05, 0.05)
                                );
                            newItem.pose.setEulerAngles(
                                    {0.0, glm::degrees(std::atan2(dir.x, dir.y)) + rand(-2.0, 2.0), 0.0});
                        } 
                    }
                }
            } else if (genTrack != nullptr) {
                
                std::shared_ptr<TrackIntersection> t = 
                    std::dynamic_pointer_cast<TrackIntersection>(genTrack);
                
                float stopProb = rand(0.0, 1.0);

                glm::vec2 center2D = t->center.lock()->coords;
                glm::vec3 center = glm::vec3(center2D.x, 0.0f, center2D.y);
                glm::vec2 link1 = t->link1.lock()->coords;
                glm::vec2 link3 = t->link3.lock()->coords;
                glm::vec2 dir2D = glm::normalize(link1 - link3);
                glm::vec3 dir = glm::vec3(dir2D.x, 0.0f, dir2D.y);
                glm::vec3 ortho = glm::vec3(-dir2D.y, 0.0f, dir2D.x);

                if (stopProb < 0.6) {

                    Scene::Item& newItem = scene.items.emplace_back();
                    newItem.pose.position = center + dir * 0.4f - ortho * 0.2f;
                    newItem.pose.setEulerAngles({
                        0.0, 
                        glm::degrees(std::atan2(dir.x, dir.z)) + 90, 
                        0.0});

                    if (stopProb < 0.3) { 
                        newItem.name = "autotrack_stopline";
                        newItem.type = ItemType::GIVE_WAY_LINE;
                    } else {
                        newItem.name = "autotrack_givewayline";
                        newItem.type = ItemType::STOP_LINE;
                    }

                    if (rand(0.0, 1.0) < 0.5) {

                        Scene::Item& newItem = scene.items.emplace_back();
                        newItem.pose.position = center - dir * 0.2f 
                                - ortho * (0.4f + rand(-0.1, 0.1));
                        newItem.pose.setEulerAngles({
                            0.0, 
                            glm::degrees(std::atan2(ortho.x, ortho.z)), 
                            0.0});

                        newItem.name = "autotrack_dynamics_obstacle";
                        newItem.type = ItemType::DYNAMIC_OBSTACLE;
                    }
                }

                // generating arrows if necessary
                
                if (t->link4.lock() == controlPoints.back()) {

                    Scene::Item& newItem = scene.items.emplace_back();
                    newItem.pose.position = center + dir * (0.9f + rand(0.05, 0.25))
                            - ortho * (0.2f + rand(-0.02, 0.02));
                    newItem.pose.setEulerAngles({
                        0.0, 
                        glm::degrees(std::atan2(ortho.x, ortho.z)) + 90, 
                        0.0});

                    newItem.name = "autotrack_arrow_left";
                    newItem.type = ItemType::GROUND_ARROW_LEFT;
                }

                if (t->link2.lock() == controlPoints.back()) {

                    Scene::Item& newItem = scene.items.emplace_back();
                    newItem.pose.position = center + dir * (0.9f + rand(0.05, 0.25))
                            - ortho * (0.2f + rand(-0.02, 0.02));
                    newItem.pose.setEulerAngles({
                        0.0, 
                        glm::degrees(std::atan2(ortho.x, ortho.z)) + 90, 
                        0.0});

                    newItem.name = "autotrack_arrow_left";
                    newItem.type = ItemType::GROUND_ARROW_RIGHT;
                }
            }

            cleanupItems(scene); 
        }
    }
}
