#include "modules/VisModule.h"

VisModule::VisModule() {

    ringModel = std::make_shared<Model>("models/ring.obj");
    circleModel = std::make_shared<Model>("models/marker.obj");
    lineModel = std::make_shared<Model>("models/ground.obj");
}

void VisModule::drawRing(GLuint shaderProgramId, glm::vec3 position, float scale, glm::vec3 color) {

    // TODO: nearly identical to drawCircle, remove duplication

    circleModel->material.Ka = objl::Vector3(color.r, color.g, color.b);
    circleModel->material.Kd = objl::Vector3(color.r, color.g, color.b);
    circleModel->material.Ks = objl::Vector3(color.r, color.g, color.b);
    
    glm::mat4 modelMat = glm::translate(glm::mat4(1.0f), position);
    modelMat = glm::scale(modelMat, glm::vec3(scale, scale, scale));
    ringModel->render(shaderProgramId, modelMat);
}

void VisModule::drawCircle(GLuint shaderProgramId, glm::vec3 position, float scale, glm::vec3 color) {

    circleModel->material.Ka = objl::Vector3(color.r, color.g, color.b);
    circleModel->material.Kd = objl::Vector3(color.r, color.g, color.b);
    circleModel->material.Ks = objl::Vector3(color.r, color.g, color.b);
    
    glm::mat4 modelMat = glm::translate(glm::mat4(1.0f), position);
    modelMat = glm::scale(modelMat, glm::vec3(scale, scale, scale));
    circleModel->render(shaderProgramId, modelMat);
}

void VisModule::drawLine(GLuint shaderProgramId, glm::vec2 start, glm::vec2 end, float width, glm::vec3 color) {

    lineModel->material.Ka = objl::Vector3(color.r, color.g, color.b);
    lineModel->material.Kd = objl::Vector3(color.r, color.g, color.b);
    lineModel->material.Ks = objl::Vector3(color.r, color.g, color.b);

    glm::vec2 distVec = end - start;
    glm::vec2 middelVec = start + distVec * 0.5f;
    float angle = atan2(distVec.y, distVec.x);

    glm::mat4 modelMat = glm::mat4(1.0f);
    modelMat = glm::translate(modelMat, glm::vec3(middelVec.x, 0.005, middelVec.y));
    modelMat = glm::rotate(modelMat, angle, glm::vec3(0, 1, 0));
    modelMat = glm::scale(modelMat, glm::vec3(width, 1.0, glm::length(distVec) * 0.5));
    lineModel->render(shaderProgramId, modelMat);
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

void VisModule::renderPositionTrace(GLuint shaderProgramId, uint64_t simulationTime, bool fancy) {

    GLuint billboardLocation = 
        glGetUniformLocation(shaderProgramId, "billboard");
    glUniform1i(billboardLocation, true);

    GLuint lightingLocation = 
        glGetUniformLocation(shaderProgramId, "lighting");
    glUniform1i(lightingLocation, false);

    for (StampedPosition& pos : tracedPositions) {

        float t = (float)(simulationTime - pos.time) * 0.01;

        float scale = 0.05;
        glm::vec3 color(0.0f, 1.0f, 0.0f);

        if (fancy) {
            scale = 0.05 + 0.02 * std::sin(t);
            color = glm::vec3(
                    0.5 + 0.5 * std::sin(t),
                    0.5 + 0.5 * std::sin(t + glm::radians(120.0f)),
                    0.5 + 0.5 * std::sin(t + glm::radians(240.0f)));
        }

        drawCircle(shaderProgramId, pos.position, scale, color);
    }

    glUniform1i(lightingLocation, true);
    glUniform1i(billboardLocation, false);
}

void VisModule::renderVisualization(
        GLuint shaderProgramId,
        Scene::Visualization& visualization,
        Scene::Settings& settings) {

    GLuint lightingLocation = 
        glGetUniformLocation(shaderProgramId, "lighting");
    glUniform1i(lightingLocation, false);

    if (settings.showVehicleTrajectory) {

        if (visualization.trajectoryPoints.size() > 0) {

            glm::vec2 prevTrajectoryPoint = visualization.trajectoryPoints[0];

            for (int i = 1; i < 128; i++) {

                glm::vec2 trajectoryPoint = visualization.trajectoryPoints[i];

                drawLine(
                        shaderProgramId,
                        prevTrajectoryPoint,
                        trajectoryPoint,
                        0.01,
                        glm::vec3(1, 0, 0));

                prevTrajectoryPoint = trajectoryPoint;
            }

            GLuint billboardLocation = 
                glGetUniformLocation(shaderProgramId, "billboard");
            glUniform1i(billboardLocation, true);

            glm::vec2 pos = visualization.trajectoryPoints[127];
            glm::vec3 endPoint = glm::vec3(pos.x, 0.005, pos.y);

            drawCircle(
                    shaderProgramId,
                    endPoint,
                    0.1,
                    glm::vec3(1, 1, 0));

            glUniform1i(billboardLocation, false);
        }
    }


    glUniform1i(lightingLocation, true);
}
