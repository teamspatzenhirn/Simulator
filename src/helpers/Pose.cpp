#include "Pose.h"

Pose::Pose() : position{0, 0, 0}, rotation{1, 0, 0, 0}, scale{1, 1, 1} {
}

Pose::Pose(glm::vec3 position)
    : Pose(position.x, position.y, position.z) {
}

Pose::Pose(float x, float y, float z) 
    : position{x, y, z}, rotation{1, 0, 0, 0}, scale{1, 1, 1} {
}

glm::vec3 Pose::getEulerAngles() {

    glm::vec3 eulerAngles;

    /*
     * We are using an alternative quaternion to euler conversion here, as
     * the glm implementation does not output full continuous 360 degrees.
     *
     * Source:
     * http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/
     */

    float test = rotation.x * rotation.y + rotation.z * rotation.w;

    if (test > 0.49999) { 
        // singularity at north pole
        eulerAngles.y = 2 * std::atan2(rotation.x,rotation.w);
        eulerAngles.z = (float)(M_PI) / 2.0f;
        eulerAngles.x = 0;
    } else if (test < -0.49999) { 
        // singularity at south pole
        eulerAngles.y = -2 * std::atan2(rotation.x,rotation.w);
        eulerAngles.z = (float)(-M_PI) / 2.0f;
        eulerAngles.x = 0;
    } else {
        float sqx = rotation.x*rotation.x;
        float sqy = rotation.y*rotation.y;
        float sqz = rotation.z*rotation.z;

        eulerAngles.z = std::asin(2*test);
        eulerAngles.y = std::atan2(2*rotation.y*rotation.w
                - 2*rotation.x*rotation.z , 1 - 2*sqy - 2*sqz);
        eulerAngles.x = std::atan2(2*rotation.x*rotation.w
                - 2*rotation.y*rotation.z , 1 - 2*sqx - 2*sqz);
    }

    eulerAngles.x = glm::degrees(eulerAngles.x);
    eulerAngles.y = glm::degrees(eulerAngles.y);
    eulerAngles.z = glm::degrees(eulerAngles.z);

    float eps = 0.0001f;

    if (std::abs(eulerAngles.x) < eps) {
        eulerAngles.x = 0.0f;
    }
    if (std::abs(eulerAngles.y) < eps) {
        eulerAngles.y = 0.0f;
    }
    if (std::abs(eulerAngles.z) < eps) {
        eulerAngles.z = 0.0f;
    }

    glm::vec3 flippedEulerAngles;

    flippedEulerAngles.x = std::fmod(180.0f - eulerAngles.x, 360.0f);
    flippedEulerAngles.y = std::fmod(180.0f - eulerAngles.y, 360.0f);
    flippedEulerAngles.z = std::fmod(180.0f - eulerAngles.z, 360.0f);

    if (glm::length(flippedEulerAngles) <= glm::length(eulerAngles)) {
        eulerAngles = flippedEulerAngles;
    }

    while (eulerAngles.x > 180) {
        eulerAngles.x -= 360;
    }
    while (eulerAngles.x <= -180) {
        eulerAngles.x += 360;
    }

    while (eulerAngles.y > 180) {
        eulerAngles.y -= 360;
    }
    while (eulerAngles.y <= -180) {
        eulerAngles.y += 360;
    }

    while (eulerAngles.z > 180) {
        eulerAngles.z -= 360;
    }
    while (eulerAngles.z <= -180) {
        eulerAngles.z += 360;
    }

    return eulerAngles;
}

void Pose::setEulerAngles(glm::vec3 eulerAngles) {

    eulerAngles.x = glm::radians(eulerAngles.x);
    eulerAngles.y = glm::radians(eulerAngles.y);
    eulerAngles.z = glm::radians(eulerAngles.z);

    /*
     * Also the glm conversion from euler angles to quaternion seems instable.
     * So we are using an alternative conversion here as well.
     *
     * Source:
     * http://www.euclideanspace.com/maths/geometry/rotations/conversions/eulerToQuaternion/
     */
    float c1 = std::cos(eulerAngles.y / 2.0f);
    float s1 = std::sin(eulerAngles.y / 2.0f);
    float c2 = std::cos(eulerAngles.z / 2.0f);
    float s2 = std::sin(eulerAngles.z / 2.0f);
    float c3 = std::cos(eulerAngles.x / 2.0f);
    float s3 = std::sin(eulerAngles.x / 2.0f);
    float c1c2 = c1*c2;
    float s1s2 = s1*s2;

    rotation.w = c1c2*c3 - s1s2*s3;
    rotation.x = c1c2*s3 + s1s2*c3;
    rotation.y = s1*c2*c3 + c1*s2*s3;
    rotation.z = c1*s2*c3 - s1*c2*s3;
}

glm::mat4 Pose::getMatrix() {

    glm::mat4 matrix(1.0f);

    matrix = glm::translate(matrix, position);
    matrix *= glm::mat4_cast(rotation);
    matrix = glm::scale(matrix, scale);

    return matrix;
}

glm::mat4 Pose::getInverseMatrix() {

    glm::mat4 matrix(1.0f);

    matrix = glm::scale(matrix, 1.0f / scale);
    matrix *= glm::mat4_cast(inverse(rotation));
    matrix = glm::translate(matrix, -position);

    return matrix;
}

Pose Pose::transform(Pose other) {

    glm::mat4 mat = other.getMatrix();        

    Pose pose;
    pose.position = glm::vec3(mat * glm::vec4(position, 1.0f));
    pose.rotation = other.rotation * rotation;
    pose.scale = other.scale * scale;

    return pose;
}
