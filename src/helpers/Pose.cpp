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

    double test = rotation.x * rotation.y + rotation.z * rotation.w;

    if (test > 0.49999) { 
        // singularity at north pole
        eulerAngles.y = 2 * atan2(rotation.x,rotation.w);
        eulerAngles.z = M_PI / 2.0f;
        eulerAngles.x = 0;
    } else if (test < -0.49999) { 
        // singularity at south pole
        heading = -2 * atan2(rotation.x,rotation.w);
        eulerAngles.z = -M_PI / 2.0f;
        eulerAngles.x = 0;
    } else {
        double sqx = rotation.x*rotation.x;
        double sqy = rotation.y*rotation.y;
        double sqz = rotation.z*rotation.z;

        eulerAngles.z = asin(2*test);
        eulerAngles.y = atan2(2*rotation.y*rotation.w
                - 2*rotation.x*rotation.z , 1 - 2*sqy - 2*sqz);
        eulerAngles.x = atan2(2*rotation.x*rotation.w
                - 2*rotation.y*rotation.z , 1 - 2*sqx - 2*sqz);
    }

    eulerAngles.x = glm::degrees(eulerAngles.x);
    eulerAngles.y = glm::degrees(eulerAngles.y);
    eulerAngles.z = glm::degrees(eulerAngles.z);

    float eps = 0.0001;

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

    double c1 = cos(eulerAngles.y / 2.0f);
    double s1 = sin(eulerAngles.y / 2.0f);
    double c2 = cos(eulerAngles.z / 2.0f);
    double s2 = sin(eulerAngles.z / 2.0f);
    double c3 = cos(eulerAngles.x / 2.0f);
    double s3 = sin(eulerAngles.x / 2.0f);
    double c1c2 = c1*c2;
    double s1s2 = s1*s2;

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
