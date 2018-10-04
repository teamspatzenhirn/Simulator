#ifndef INC_2019_CAMERA_H
#define INC_2019_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Pose.h"

class Camera {

public:

    Camera();
    Camera(float fov, float aspectRatio);

    float fov;
    float aspectRatio;

    /*
     * Camera pose in world coordinates
     */
    Pose pose;

    glm::mat4 getProjectionMatrix();

    /*
     * This sets the uniform variables "view" "projection"
     * and "cameraPosition" in the currently bound shader program.
     */
    void render(GLuint shaderProgramId);

    /*
     * This function will return a vector, from the origin of the camera through
     * a given point (x, y) on the near plane. Here w and h are the width and
     * height of the current viewport in pixels.
     */
    glm::vec3 pickRay(double x, double y, int windowWidth, int windowHeight);
};

#endif
