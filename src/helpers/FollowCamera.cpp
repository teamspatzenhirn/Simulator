#include "FollowCamera.h"

#include <iostream>

void FollowCamera::update(Pose& followPose) {

    for (ScrollEvent& scrollEvent : getScrollEvents()) {

        if (scrollEvent.yoffset < 0) { 
            height = std::min(20.0f, height * 1.2f);
        } else {
            height = std::max(0.5f, height * 0.8f);
        }
    }

    pose.position.x = followPose.position.x;
    pose.position.z = followPose.position.z;
    pose.position.y = height;

    pose.rotation = followPose.rotation;
    pose.rotation = glm::rotate(
        pose.rotation, (float)-M_PI, glm::vec3(0.0f, 1.0f, 0.0f));
    pose.rotation = glm::rotate(
        pose.rotation, -1.56f, glm::vec3(1.0f, 0.0f, 0.0f));
}
