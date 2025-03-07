//
// Created by maximilianglumann on 3/7/25.
//

#ifndef SPATZSIM_ORTHOCAMERA_H
#define SPATZSIM_ORTHOCAMERA_H


#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GL/glew.h>

#include "Input.h"
#include "Camera.h"

class OrthoCamera : public Camera {

    float prevMouseX;

public:
    glm::mat4 getProjectionMatrix();

    /*
     * The two rotation parameters are stored separate
     * so that the roation about the x axis can be limited
     * easier. Angles of more then +-90 degrees of pitch
     * result in quite weird camera behaviour.
     */

    // rotation about y axis
    float yaw;

    OrthoCamera();
    OrthoCamera(float fov, float aspectRatio);
    OrthoCamera(glm::vec3 position, float yaw, float fov, float aspectRatio);

    /*
     * Call this method to read in keyboard and mouse
     * input and update the view matrix accordingly.
     * Use W A S D, Shift, Space and the mouse to move.
     */
    void update(GLFWwindow* window, float dt);
};

#endif //SPATZSIM_ORTHOCAMERA_H
