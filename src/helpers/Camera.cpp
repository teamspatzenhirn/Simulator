#include "Camera.h"

Camera::Camera(float fov, float aspectRatio) {

    projection = glm::perspective(fov, aspectRatio, 0.1f, 100.0f);
    view = glm::mat4(1.0f);
}

void Camera::render(GLuint shaderProgramId) {

    GLint viewLocation =
        glGetUniformLocation(shaderProgramId, "view");
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

    GLint projectionLocation =
        glGetUniformLocation(shaderProgramId, "projection");
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

    // TODO: inefficient!
    glm::mat4 iv = glm::inverse(view);
    glm::vec4 cameraPosition = iv * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    GLint posLocation =
        glGetUniformLocation(shaderProgramId, "cameraPosition");
    glUniform3fv(posLocation, 1, &cameraPosition[0]);
}

glm::vec3 Camera::pickRay(GLuint x, GLuint y, GLuint viewportWidth, GLuint viewportHeight) {

    glm::vec4 clipCoords(
            2.0f * x / viewportWidth - 1.0f,
            1.0f - 2.0f * y / viewportHeight,
            -1.0f,
            1.0f);
    
    glm::vec4 cameraCoords = glm::inverse(projection) * clipCoords;
    glm::vec4 worldCoords = glm::inverse(view) *
        glm::vec4(cameraCoords.x, cameraCoords.y, -1.0f, 0.0f);

    return glm::normalize(glm::vec3(worldCoords.x, worldCoords.y, worldCoords.z));
}
