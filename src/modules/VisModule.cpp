#include "modules/VisModule.h"

VisModule::VisModule() {
}

void VisModule::drawModel(
        GLuint shaderProgramId, 
        Model& model, 
        glm::vec3 position, 
        float scale, 
        glm::vec3 color) {

    Model::Material savedMaterial = model.material;

    model.material.ka = color;
    model.material.kd = color;
    model.material.ks = color;
    
    glm::mat4 modelMat = glm::translate(glm::mat4(1.0f), position);
    modelMat = glm::scale(modelMat, glm::vec3(scale, scale, scale));
    model.render(shaderProgramId, modelMat);
    
    model.material = savedMaterial;
}

void VisModule::drawLine(
        GLuint shaderProgramId, 
        Model& model, 
        glm::vec2 start, 
        glm::vec2 end, 
        float width, 
        glm::vec3 color) {

    drawLine(
            shaderProgramId, 
            model, 
            glm::vec3(start.x, 0.005f, start.y), 
            glm::vec3(end.x, 0.005f, end.y), 
            width, 
            color);
}

void VisModule::drawLine(
        GLuint shaderProgramId, 
        Model& lineModel, 
        glm::vec3 start, 
        glm::vec3 end, 
        float width, 
        glm::vec3 color) {

    lineModel.material.ka = color;
    lineModel.material.kd = color;
    lineModel.material.ks = color;

    glm::vec3 distVec = end - start;
    glm::vec3 middelVec = start + distVec * 0.5f;
    float angleY = atan2f(distVec.x, distVec.z);
    float angleX = atan2f(distVec.y, distVec.z);

    glm::mat4 modelMat = glm::mat4(1.0f);
    modelMat = glm::translate(modelMat, middelVec);
    modelMat = glm::rotate(modelMat, angleY, glm::vec3(0, 1, 0));
    modelMat = glm::rotate(modelMat, angleX, glm::vec3(1, 0, 0));
    modelMat = glm::scale(modelMat, glm::vec3(width, 1.0, glm::length(distVec) * 0.5));
    lineModel.render(shaderProgramId, modelMat);
}

void VisModule::drawArrow(
        GLuint shaderProgramId, 
        Model& arrowModel, 
        glm::vec3 start, 
        glm::vec3 end, 
        float scale, 
        glm::vec3 color) {

    arrowModel.material.ka = color;
    arrowModel.material.kd = color;
    arrowModel.material.ks = color;

    glm::vec3 distVec = end - start;
    glm::lookAt(start, distVec, glm::vec3(0, 1, 0));
    float angleZ = -atan2f(distVec.x, distVec.z);

    // TODO: rotation incomplete!

    glm::mat4 modelMat = glm::mat4(1.0f);
    modelMat = glm::translate(modelMat, start);
    modelMat = glm::rotate(modelMat, (float) M_PI/2, glm::vec3(1, 0, 0));
    modelMat = glm::rotate(modelMat, angleZ, glm::vec3(0, 0, 1));
    modelMat = glm::scale(modelMat, glm::vec3(scale, glm::length(end - start) * 0.5, scale));
    arrowModel.render(shaderProgramId, modelMat);
}

void VisModule::addPoseTrace(Pose& pose, double simulationTime) {

    if (simulationTime - lastTraceTime > 0.050 
            || simulationTime < lastTraceTime) {

        if (tracedPoses.size() > 100) {
            tracedPoses.pop_front();
        }

        tracedPoses.push_back({simulationTime, pose});
        lastTraceTime = simulationTime;
    }
}

void VisModule::renderPoseTrace(GLuint shaderProgramId, Model& pointModel, double simulationTime, bool fancy) {

    GLint billboardLocation = 
        glGetUniformLocation(shaderProgramId, "billboard");
    glUniform1i(billboardLocation, true);

    GLint lightingLocation = 
        glGetUniformLocation(shaderProgramId, "lighting");
    glUniform1i(lightingLocation, false);

    for (StampedPose& pose : tracedPoses) {

        float t = (float)(simulationTime - pose.time) * 10.0f;

        float scale = 1.0f;
        glm::vec3 color(0.0f, 1.0f, 0.0f);

        if (fancy) {
            scale = 0.05f + 0.02f * std::sin(t);
            color = glm::vec3(
                    0.5f + 0.5f * std::sin(t),
                    0.5f + 0.5f * std::sin(t + glm::radians(120.0f)),
                    0.5f + 0.5f * std::sin(t + glm::radians(240.0f)));
        }

        drawModel(shaderProgramId, pointModel, pose.pose.position, scale, color);
    }

    glUniform1i(lightingLocation, true);
    glUniform1i(billboardLocation, false);
}


void VisModule::renderDynamicItems(
        GLuint shaderProgramId,
        Model& arrowModel,
        double simulationTime, 
        std::vector<Scene::Item>& items) {

    float offset = std::sin((float)simulationTime * 0.01f) * 0.01f;

    for (Scene::Item& item : items) {

        if (item.type == DYNAMIC_OBSTACLE) {

            glm::vec4 start(0, 0.05, 0.17 + offset, 1); 
            glm::vec4 end(0, 0.05, 0.23 + offset, 1);
            glm::mat4 mat = item.pose.getMatrix();

            glm::vec3 startWorld(mat * start);
            glm::vec3 endWorld(mat * end);

            drawArrow(shaderProgramId, arrowModel, 
                    startWorld, endWorld, 0.05f, glm::vec3(1, 1, 0));
        } 

        if (item.type == DYNAMIC_PEDESTRIAN_RIGHT) {

            glm::vec4 start(0.075 - offset, 0.075, 0.0, 1); 
            glm::vec4 end(0.12 - offset, 0.075, 0.0, 1);
            glm::mat4 mat = item.pose.getMatrix();

            glm::vec3 startWorld(mat * start);
            glm::vec3 endWorld(mat * end);

            drawArrow(shaderProgramId, arrowModel,
                    startWorld, endWorld, 0.05f, glm::vec3(1, 1, 0));
        } 

        if (item.type == DYNAMIC_PEDESTRIAN_LEFT) {

            glm::vec4 start(-0.075 + offset, 0.075, 0.0, 1); 
            glm::vec4 end(-0.12 + offset, 0.075, 0.0, 1);
            glm::mat4 mat = item.pose.getMatrix();

            glm::vec3 startWorld(mat * start);
            glm::vec3 endWorld(mat * end);

            drawArrow(shaderProgramId, arrowModel,
                    startWorld, endWorld, 0.05f, glm::vec3(1, 1, 0));
        } 
    }

}

void VisModule::renderSensors(
        GLuint shaderProgramId, 
        Model& lineModel, 
        Model& markerModel, 
        Car& car, 
        Settings& settings) {

    GLint lightingLocation = 
        glGetUniformLocation(shaderProgramId, "lighting");
    glUniform1i(lightingLocation, false);

    GLint billboardLocation = 
        glGetUniformLocation(shaderProgramId, "billboard");
    glUniform1i(billboardLocation, true);

    glm::vec3 binaryLightSensorWorldPos = car.modelPose.getMatrix() * 
        glm::vec4(car.binaryLightSensor.pose.position, 1);

    glm::vec3 laserSensorWorldPos = car.modelPose.getMatrix() * 
        glm::vec4(car.laserSensor.pose.position, 1);

    if (settings.showBinaryLightSensor) {
        drawModel(
                shaderProgramId,
                markerModel,
                binaryLightSensorWorldPos,
                0.05f,
                glm::vec3(1, 1, 0));
    }

    if (settings.showLaserSensor) {
        drawModel(
                shaderProgramId,
                markerModel,
                laserSensorWorldPos,
                0.05f,
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
                lineModel,
                binaryLightSensorWorldPos,
                binaryLightSensorWorldPos + glm::vec3(glm::normalize(dir)) * lineLength,
                0.005f,
                color);
    }

    lineLength = std::min(2.0f, car.laserSensor.value);

    if (settings.showLaserSensor) {
        drawLine(
                shaderProgramId,
                lineModel,
                laserSensorWorldPos,
                laserSensorWorldPos + glm::vec3(glm::normalize(dir)) * lineLength,
                0.005f,
                glm::vec3(1, 0, 0));
    }

    glUniform1i(lightingLocation, true);
}

void VisModule::renderTrackPath(
        GLuint shaderProgramId,
        Model& pointModel,
        Tracks& tracks) { 

    GLint lightingLocation = 
        glGetUniformLocation(shaderProgramId, "lighting");
    glUniform1i(lightingLocation, false);

    GLint billboardLocation = 
        glGetUniformLocation(shaderProgramId, "billboard");
    glUniform1i(billboardLocation, true);

    std::vector<glm::vec2> path = tracks.getPath(0.1);

    for (glm::vec2& point : path) { 
        drawModel(shaderProgramId, 
                pointModel, 
                glm::vec3(point.x, 0.1, point.y), 
                0.1, 
                glm::vec3(1, 0, 1));
    }

    glUniform1i(billboardLocation, false);

    glUniform1i(lightingLocation, true);
}

void VisModule::renderVisualization(
        GLuint shaderProgramId,
        Model& lineModel,
        Model& endPointModel,
        Scene::Visualization& visualization,
        Settings& settings) {

    GLint lightingLocation = 
        glGetUniformLocation(shaderProgramId, "lighting");
    glUniform1i(lightingLocation, false);

    if (settings.showVehicleTrajectory) {

        if (visualization.trajectoryPoints.size() > 0) {

            glm::vec2 prevTrajectoryPoint = visualization.trajectoryPoints[0];

            for (size_t i = 1; i < 128; i++) {

                glm::vec2 trajectoryPoint = visualization.trajectoryPoints[i];

                drawLine(
                        shaderProgramId,
                        lineModel,
                        prevTrajectoryPoint,
                        trajectoryPoint,
                        0.01f,
                        glm::vec3(1, 0, 0));

                prevTrajectoryPoint = trajectoryPoint;
            }

            GLint billboardLocation = 
                glGetUniformLocation(shaderProgramId, "billboard");
            glUniform1i(billboardLocation, true);

            glm::vec2 pos = visualization.trajectoryPoints[127];
            glm::vec3 endPoint = glm::vec3(pos.x, 0.005, pos.y);

            drawModel(
                    shaderProgramId,
                    endPointModel,
                    endPoint,
                    0.1f,
                    glm::vec3(1, 1, 0));

            glUniform1i(billboardLocation, false);
        }
    }

    glUniform1i(lightingLocation, true);
}
