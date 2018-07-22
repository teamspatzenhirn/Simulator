#include "PointLight.h"

PointLight::PointLight() : PointLight(0.0f, 0.0f, 0.0f) {
}

PointLight::PointLight(float x, float y, float z) : x(x), y(y), z(z) {

    ambientColor = glm::vec3(0.3f);
    diffuseColor = glm::vec3(0.7f);
    specularColor = glm::vec3(1.0f);
}

void PointLight::render(GLuint shaderProgramId) {

    GLuint lightPositionLocation =
        glGetUniformLocation(shaderProgramId, "lightPosition");
    GLuint ambientColorLocation = 
        glGetUniformLocation(shaderProgramId, "ia");
    GLuint diffuseColorLocation = 
        glGetUniformLocation(shaderProgramId, "id");
    GLuint specularColorLocation = 
        glGetUniformLocation(shaderProgramId, "is");

    glUniform3f(lightPositionLocation, x, y, z);
    glUniform3fv(ambientColorLocation, 1, &ambientColor[0]);
    glUniform3fv(diffuseColorLocation, 1, &diffuseColor[0]);
    glUniform3fv(specularColorLocation, 1, &specularColor[0]);
}
