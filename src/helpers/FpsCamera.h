#ifndef INC_2019_FPSCAMERA_H
#define INC_2019_FPSCAMERA_H

#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GL/glew.h>

#include "Input.h"
#include "Camera.h"

class FpsCamera : public Camera {

    float prevMouseX;
    float prevMouseY;

public:

    /*
     * The two rotation parameters are stored separate
     * so that the roation about the x axis can be limited
     * easier. Angles of more then +-90 degrees of pitch
     * result in quite weird camera behaviour.
     */

    // rotation about x axis
    float pitch;
    // rotation about y axis
    float yaw;

    FpsCamera();
    FpsCamera(float fov, float aspectRatio);

    /*
     * Call this method to read in keyboard and mouse
     * input and update the view matrix accordingly.
     * Use W A S D, Shift, Space and the mouse to move.
     */
    void update(GLFWwindow* window, float dt);
};

#endif
