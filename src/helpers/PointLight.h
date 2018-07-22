#ifndef INC_2019_POINTLIGHT_H
#define INC_2019_POINTLIGHT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class PointLight {

public:

    PointLight();
    PointLight(float x, float y, float z);

    float x;
    float y;
    float z;

    glm::vec3 ambientColor;
    glm::vec3 diffuseColor;
    glm::vec3 specularColor;

    /*
     * Activates this point light for rendering.
     */
    void render(GLuint shaderProgramId);
};

#endif
