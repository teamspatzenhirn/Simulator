#ifndef INC_2019_POSE_H
#define INC_2019_POSE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Pose {

public:

    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    Pose();
    Pose(glm::vec3 position);
    Pose(float x, float y, float z);

    glm::mat4 getMatrix();
};

#endif
