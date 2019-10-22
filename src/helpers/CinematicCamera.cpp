/**
 * @file CinematicCamera.cpp
 * @author jonas
 * @date 10/22/19
 * Camera that is following the car with smooth movement
 */

#include "CinematicCamera.h"

void CinematicCamera::update(Pose &carPose) {

    auto behindCar = carPose.position + glm::vec3{0, 0.75, 0} - carPose.rotation * glm::vec3{0, 0, 1};

    pose.position += 0.005f * (behindCar - pose.position);

    // Look at car
    pose.rotation = glm::quatLookAt(glm::normalize(carPose.position - pose.position), {0, 1, 0});
}
