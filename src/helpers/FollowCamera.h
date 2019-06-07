#ifndef INC_2019_FOLLOW_CAMERA_H
#define INC_2019_FOLLOW_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"

#include "helpers/Input.h"

class FollowCamera : public Camera {

public:

    float height = 5.0f;

    void update(Pose& followPose);
};

#endif
