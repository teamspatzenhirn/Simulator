#ifndef INC_2019_POSE_H
#define INC_2019_POSE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

/*
 * Because position, rotation and scale of an object are often
 * used in combination, it is convenient to bundle them. This
 * type is called "Pose" (even though "Transformation" would be
 * more accurate), as this name is much shorter.
 *
 * Pose is intentionally designed as a struct with only public
 * variables. While other implementations may use getter and
 * setter functions to maintain a valid (private) transformation
 * matrix, here position, rotation and scale fields are freely
 * accessible, which makes calculations way less convoluted then
 * when using getter and setter functions. A transformation matrix
 * is calculated on-demand by using get(Inverted)Matrix().
 */
struct Pose {

    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;

    Pose();
    explicit Pose(glm::vec3 position);
    Pose(float x, float y, float z);

    glm::vec3 getEulerAngles();
    void setEulerAngles(glm::vec3 eulerAngles);

    glm::mat4 getMatrix();
    glm::mat4 getInverseMatrix();

    Pose transform(Pose other);
};

#endif
