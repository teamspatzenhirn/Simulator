#include "modules/VisModule.h"

VisModule::VisModule() {

    ringModel = std::make_shared<Model>("models/ring.obj");
    circleModel = std::make_shared<Model>("models/marker.obj");
    lineModel = std::make_shared<Model>("models/ground.obj");
    arrowModel = std::make_shared<Model>("models/arrow.obj");
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

    drawLine(shaderProgramId, glm::vec3(start.x, 0.005f, start.y), glm::vec3(end.x, 0.005f, end.y), width, color);
}

void VisModule::drawLine(GLuint shaderProgramId, glm::vec3 start, glm::vec3 end, float width, glm::vec3 color) {

    lineModel->material.Ka = objl::Vector3(color.r, color.g, color.b);
    lineModel->material.Kd = objl::Vector3(color.r, color.g, color.b);
    lineModel->material.Ks = objl::Vector3(color.r, color.g, color.b);

    glm::vec3 distVec = end - start;
    glm::vec3 middelVec = start + distVec * 0.5f;
    float angleY = atan2(distVec.x, distVec.z);
    float angleX = atan2(distVec.y, distVec.z);

    glm::mat4 modelMat = glm::mat4(1.0f);
    modelMat = glm::translate(modelMat, middelVec);
    modelMat = glm::rotate(modelMat, angleY, glm::vec3(0, 1, 0));
    modelMat = glm::rotate(modelMat, angleX, glm::vec3(1, 0, 0));
    modelMat = glm::scale(modelMat, glm::vec3(width, 1.0, glm::length(distVec) * 0.5));
    lineModel->render(shaderProgramId, modelMat);
}

void VisModule::drawArrow(GLuint shaderProgramId, glm::vec3 start, glm::vec3 end, float scale, glm::vec3 color) {

    arrowModel->material.Ka = objl::Vector3(color.r, color.g, color.b);
    arrowModel->material.Kd = objl::Vector3(color.r, color.g, color.b);
    arrowModel->material.Ks = objl::Vector3(color.r, color.g, color.b);

    glm::vec3 distVec = end - start;
    glm::lookAt(start, distVec, glm::vec3(0, 1, 0));
    float angleZ = -atan2(distVec.x, distVec.z);

    // TODO: rotation incomplete!

    glm::mat4 modelMat = glm::mat4(1.0f);
    modelMat = glm::translate(modelMat, start);
    modelMat = glm::rotate(modelMat, (float) M_PI/2, glm::vec3(1, 0, 0));
    modelMat = glm::rotate(modelMat, angleZ, glm::vec3(0, 0, 1));
    modelMat = glm::scale(modelMat, glm::vec3(scale, glm::length(end - start) * 0.5, scale));
    arrowModel->render(shaderProgramId, modelMat);
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


void VisModule::renderDynamicItems(
        GLuint shaderProgramId,
        double simulationTime, 
        std::vector<std::shared_ptr<Scene::Item>>& items) {

    float offset = std::sin(simulationTime * 0.01) * 0.01;

    for (std::shared_ptr<Scene::Item>& item : items) {

        if (item->type == DYNAMIC_OBSTACLE) {

            glm::vec4 start(0, 0.05, 0.17 + offset, 1); 
            glm::vec4 end(0, 0.05, 0.23 + offset, 1);
            glm::mat4 mat = item->pose.getMatrix();

            glm::vec3 startWorld(mat * start);
            glm::vec3 endWorld(mat * end);

            drawArrow(shaderProgramId, startWorld, endWorld, 0.05, glm::vec3(1, 1, 0));
        } 

        if (item->type == DYNAMIC_PEDESTRIAN_RIGHT) {

            glm::vec4 start(0.075 - offset, 0.075, 0.0, 1); 
            glm::vec4 end(0.12 - offset, 0.075, 0.0, 1);
            glm::mat4 mat = item->pose.getMatrix();

            glm::vec3 startWorld(mat * start);
            glm::vec3 endWorld(mat * end);

            drawArrow(shaderProgramId, startWorld, endWorld, 0.05, glm::vec3(1, 1, 0));
        } 

        if (item->type == DYNAMIC_PEDESTRIAN_LEFT) {

            glm::vec4 start(-0.075 + offset, 0.075, 0.0, 1); 
            glm::vec4 end(-0.12 + offset, 0.075, 0.0, 1);
            glm::mat4 mat = item->pose.getMatrix();

            glm::vec3 startWorld(mat * start);
            glm::vec3 endWorld(mat * end);

            drawArrow(shaderProgramId, startWorld, endWorld, 0.05, glm::vec3(1, 1, 0));
        } 
    }

}

void VisModule::renderSensors(GLuint shaderProgramId, Scene::Car& car, Settings& settings) {

    GLuint lightingLocation = 
        glGetUniformLocation(shaderProgramId, "lighting");
    glUniform1i(lightingLocation, false);

    GLuint billboardLocation = 
        glGetUniformLocation(shaderProgramId, "billboard");
    glUniform1i(billboardLocation, true);

    glm::vec3 binaryLightSensorWorldPos = car.modelPose.getMatrix() * 
        glm::vec4(car.binaryLightSensor.pose.position, 1);

    glm::vec3 laserSensorWorldPos = car.modelPose.getMatrix() * 
        glm::vec4(car.laserSensor.pose.position, 1);

    if (settings.showBinaryLightSensor) {
        drawCircle(
                shaderProgramId,
                binaryLightSensorWorldPos,
                0.05,
                glm::vec3(1, 1, 0));
    }

    if (settings.showLaserSensor) {
        drawCircle(
                shaderProgramId,
                laserSensorWorldPos,
                0.05,
                glm::vec3(1, 1, 0));
    }

    glm::vec4 dir{-1, 0, 0, 0};
    dir = car.modelPose.getMatrix() * dir;

    glUniform1i(billboardLocation, false);

    float lineLength = std::min(2.0f, car.binaryLightSensor.value);

    if (settings.showBinaryLightSensor) {

        glm::vec3 color =
            car.binaryLightSensor.triggered
                ? glm::vec3(0, 1, 0): glm::vec3(1, 0, 0);

        drawLine(
                shaderProgramId,
                binaryLightSensorWorldPos,
                binaryLightSensorWorldPos + glm::vec3(glm::normalize(dir)) * lineLength,
                0.005,
                color);
    }

    lineLength = std::min(2.0f, car.laserSensor.value);

    if (settings.showLaserSensor) {
        drawLine(
                shaderProgramId,
                laserSensorWorldPos,
                laserSensorWorldPos + glm::vec3(glm::normalize(dir)) * lineLength,
                0.005,
                glm::vec3(1, 0, 0));
    }

    glUniform1i(lightingLocation, true);
}

void VisModule::renderVisualization(
        GLuint shaderProgramId,
        Scene::Visualization& visualization,
        Settings& settings) {

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
