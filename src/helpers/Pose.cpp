#include "Pose.h"

Pose::Pose() : position{0, 0, 0}, rotation{0, 0, 0}, scale{1, 1, 1} {
}

Pose::Pose(glm::vec3 position)
    : Pose(position.x, position.y, position.z) {
}

Pose::Pose(float x, float y, float z) 
    : position{position}, rotation{0, 0, 0}, scale{1, 1, 1} {
}

glm::mat4 Pose::getMatrix() {

    glm::mat4 matrix(1.0f);

    matrix = glm::translate(matrix, position);

    matrix = glm::rotate(matrix, rotation.x, glm::vec3(1.0f, 0, 0));
    matrix = glm::rotate(matrix, rotation.y, glm::vec3(0, 1.0f, 0));
    matrix = glm::rotate(matrix, rotation.z, glm::vec3(0, 0, 1.0f));

    matrix = glm::scale(matrix, scale);

    return matrix;
}
