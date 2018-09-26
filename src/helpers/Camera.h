#ifndef INC_2019_CAMERA_H
#define INC_2019_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Camera {

public:

    Camera();
    Camera(float fov, float aspectRatio);

    float fov;
    float aspectRatio;

    glm::mat4 view;

    glm::vec3 getPosition();

    glm::mat4 getProjectionMatrix();

    /*
     * This sets the uniform variables "view" "projection"
     * and "cameraPosition" in the currently bound shader program.
     */
    void render(GLuint shaderProgramId);

    /*
     * This function will return a vector, from the origin of the camera and a
     * given point (x, y) on the near plane. Here w and h are the width and
     * height of the current viewport in pixels.
     */
    glm::vec3 pickRay(double x, double y, int windowWidth, int windowHeight);
};

#endif
