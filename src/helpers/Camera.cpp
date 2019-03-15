#include "Camera.h"

Camera::Camera() : Camera((float)M_PI * 0.3f, 4.0f/3.0f) {
}

Camera::Camera(float fov, float aspectRatio) 
    : fov{fov},
      aspectRatio{aspectRatio},
      pose{0, 0, 0} {
}

glm::mat4 Camera::getProjectionMatrix() {

    return glm::perspective(fov, aspectRatio, 0.1f, 100.0f);
}

void Camera::render(GLuint shaderProgramId) {

    glm::mat4 projection = getProjectionMatrix();
    glm::mat4 view = pose.getInverseMatrix();

    GLint viewLocation =
        glGetUniformLocation(shaderProgramId, "view");
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

    GLint projectionLocation =
        glGetUniformLocation(shaderProgramId, "projection");
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

    GLint posLocation =
        glGetUniformLocation(shaderProgramId, "cameraPosition");
    glUniform3fv(posLocation, 1, glm::value_ptr(pose.position));
}

glm::vec3 Camera::pickRay(double x, double y, int windowWidth, int windowHeight) {

    glm::mat4 projection = getProjectionMatrix();
    glm::mat4 inverseView = pose.getMatrix();

    glm::vec4 clipCoords(
            2.0f * x / windowWidth - 1.0f,
            1.0f - 2.0f * y / windowHeight,
            -1.0f,
            1.0f);
    
    glm::vec4 cameraCoords = glm::inverse(projection) * clipCoords;
    glm::vec4 worldCoords = inverseView *
        glm::vec4(cameraCoords.x, cameraCoords.y, cameraCoords.z, 0.0f);

    return glm::normalize(glm::vec3(worldCoords.x, worldCoords.y, worldCoords.z));
}
