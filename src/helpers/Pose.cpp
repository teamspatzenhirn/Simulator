#include "Pose.h"

Pose::Pose() : position{0, 0, 0}, rotation{0, 0, 0, 1}, scale{1, 1, 1} {
}

Pose::Pose(glm::vec3 position)
    : Pose(position.x, position.y, position.z) {
}

Pose::Pose(float x, float y, float z) 
    : position{position}, rotation{0, 0, 0, 1}, scale{1, 1, 1} {
}

glm::mat4 Pose::getMatrix() {

    glm::mat4 matrix(1.0f);

    matrix = glm::translate(matrix, position);
    matrix *= glm::mat4_cast(rotation);
    matrix = glm::scale(matrix, scale);

    return matrix;
}
