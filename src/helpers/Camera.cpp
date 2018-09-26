#include "Camera.h"

Camera::Camera() : Camera(M_PI * 0.3, 4.0f/3.0f) {
}

Camera::Camera(float fov, float aspectRatio) 
    : fov{fov},
      aspectRatio{aspectRatio},
      view{glm::mat4(1.0f)} {
}

glm::vec3 Camera::getPosition() {

    glm::mat3 rotation(view);
    return -glm::transpose(rotation) * glm::vec3(view[3]);
}

glm::mat4 Camera::getProjectionMatrix() {

    return glm::perspective(fov, aspectRatio, 0.1f, 100.0f);
}

void Camera::render(GLuint shaderProgramId) {

    glm::mat4 projection = getProjectionMatrix();

    GLint viewLocation =
        glGetUniformLocation(shaderProgramId, "view");
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

    GLint projectionLocation =
        glGetUniformLocation(shaderProgramId, "projection");
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

    glm::vec3 cameraPosition = getPosition();

    GLint posLocation =
        glGetUniformLocation(shaderProgramId, "cameraPosition");
    glUniform3fv(posLocation, 1, &cameraPosition[0]);
}

glm::vec3 Camera::pickRay(double x, double y, int windowWidth, int windowHeight) {

    glm::mat4 projection = getProjectionMatrix();

    glm::vec4 clipCoords(
            2.0f * x / windowWidth - 1.0f,
            1.0f - 2.0f * y / windowHeight,
            -1.0f,
            1.0f);
    
    glm::vec4 cameraCoords = glm::inverse(projection) * clipCoords;
    glm::vec4 worldCoords = glm::inverse(view) *
        glm::vec4(cameraCoords.x, cameraCoords.y, cameraCoords.z, 0.0f);

    return glm::normalize(glm::vec3(worldCoords.x, worldCoords.y, worldCoords.z));
}
