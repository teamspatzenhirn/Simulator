#include "modules/VisModule.h"

VisModule::VisModule() {

    markerModel = std::make_shared<Model>("models/marker.obj");
}

void VisModule::addPositionTrace(glm::vec3 position, uint64_t simulationTime) {

    if (simulationTime - lastTraceTime > 50) {

        if (tracedPositions.size() > 200) {
            tracedPositions.pop_front();
        }

        tracedPositions.push_back({simulationTime, position});
        lastTraceTime = simulationTime;
    }
}

void VisModule::renderPositionTrace(GLuint shaderProgramId, uint64_t simulationTime) {

    GLuint billboardLocation = 
        glGetUniformLocation(shaderProgramId, "billboard");
    glUniform1i(billboardLocation, true);

    GLuint lightingLocation = 
        glGetUniformLocation(shaderProgramId, "lighting");
    glUniform1i(lightingLocation, false);

    for (StampedPosition& pos : tracedPositions) {

        float t = (float)(simulationTime - pos.time) * 0.01;

        float r = 0.5 + 0.5 * std::sin(t);
        float g = 0.5 + 0.5 * std::sin(t + glm::radians(120.0f));
        float b = 0.5 + 0.5 * std::sin(t + glm::radians(240.0f));

        markerModel->material.Ka = objl::Vector3(r, g, b);
        markerModel->material.Kd = objl::Vector3(r, g, b);
        markerModel->material.Ks = objl::Vector3(r, g, b);

        float scale = 0.05 + 0.02 * std::sin(t);

        glm::mat4 modelMat = glm::translate(glm::mat4(1.0f), pos.position);
        modelMat = glm::scale(modelMat, glm::vec3(scale, scale, scale));
        markerModel->render(shaderProgramId, modelMat);
    }

    glUniform1i(lightingLocation, true);
    glUniform1i(billboardLocation, false);
}
