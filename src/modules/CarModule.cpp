#include "CarModule.h"

CarModule::CarModule() {

    carModel.upload();
}

void CarModule::updatePosition(Scene::Car& car, float deltaTime) {

    float dt = deltaTime / 1000; // in seconds, input is milliseconds

    Scene::Car::SimulatorState& x = car.simulatorState;
    x.x1 = car.modelPose.position.z;
    x.x2 = car.modelPose.position.x;
    x.psi = glm::radians(car.modelPose.getEulerAngles().y);

    double acc = (car.vesc.velocity - x.v_lon) / dt;
    acc = std::min(acc, car.limits.max_F * car.systemParams.mass / car.systemParams.mass);
    acc = std::max(acc, -car.limits.max_F * car.systemParams.mass / car.systemParams.mass);

    double F = acc * car.systemParams.mass;
    F = std::min(F, car.limits.max_F * car.systemParams.mass);
    F = std::max(F, -car.limits.max_F * car.systemParams.mass);

    Scene::Car::SimulatorState dx;
    dx.delta = (car.vesc.steeringAngle - x.delta) / dt;
    dx.delta = std::min(dx.delta, car.limits.max_d_delta);
    dx.delta = std::max(dx.delta, -car.limits.max_d_delta);
    double alpha_front = 0, alpha_rear = 0;

    if (!car.wheels.usePacejkaModel || x.v_lon <= 0) {
        dx.x1 = x.v * std::cos(x.psi);
        dx.x2 = x.v * std::sin(x.psi);
        dx.psi = x.v * std::tan(x.delta) / car.systemParams.axesDistance;
        double cos_ = std::cos(x.delta);
        double tan_ = std::tan(x.delta);

        dx.v = ((1 + car.systemParams.axesMomentRatio * (1 / cos_ - 1)) * F
                - 2 * car.systemParams.getM() * x.v * tan_ / (cos_ * cos_) * dx.delta)
               / (car.systemParams.mass + car.systemParams.getM() * (tan_ * tan_));

        x.x1 += dt * dx.x1;
        x.x2 += dt * dx.x2;
        x.psi += dt * dx.psi;
        x.v += dt * dx.v;
        x.d_psi = dx.psi;

        x.v_lon = x.v;
        dx.v_lon = dx.v;
        dx.v_lat = 0;
    } else {
        dx.x1 = x.v_lon*std::cos(x.psi) + x.v_lat*std::sin(x.psi);
        dx.x2 = x.v_lon*std::sin(x.psi) - x.v_lat*std::cos(x.psi);
        dx.psi = x.d_psi;

        //abs um korrekten Schräglaufwinkel fürs Rückwärts fahren zu erhalten
        alpha_front = -std::atan2(dx.psi * car.systemParams.distCogToFrontAxle
                + x.v_lat, std::abs(x.v_lon)) + x.delta;
        alpha_rear = std::atan2(dx.psi * car.systemParams.distCogToRearAxle
                - x.v_lat, std::abs(x.v_lon));

        const double F_front_lat = car.systemParams.mass * car.wheels.D_front * std::sin(
                car.wheels.C_front * std::atan(car.wheels.B_front * alpha_front));

        const double F_rear_lat = car.systemParams.mass * car.wheels.D_rear * std::sin(
                car.wheels.C_rear * std::atan(car.wheels.B_rear * alpha_rear));

        const double F_front_lon = F * car.systemParams.axesMomentRatio;
        const double F_rear_lon = F * (1-car.systemParams.axesMomentRatio);

        dx.v_lon = 1 / car.systemParams.mass * (F_rear_lon - F_front_lat*std::sin(x.delta)
                + F_front_lon*std::cos(x.delta) + car.systemParams.mass*x.v_lat*x.d_psi);
        dx.v_lat = 1 / car.systemParams.mass * (F_rear_lat + F_front_lat*std::cos(x.delta)
                + F_front_lon*std::sin(x.delta) - car.systemParams.mass*x.v_lon*x.d_psi);
        dx.d_psi = 1 / car.systemParams.inertia * ((F_front_lat*std::cos(x.delta)
                + F_front_lon*std::sin(x.delta)) * car.systemParams.distCogToFrontAxle
                - F_rear_lat * car.systemParams.distCogToRearAxle);

        x.x1 += dt * dx.x1;
        x.x2 += dt * dx.x2;
        x.psi += dt * dx.psi;
        x.v_lon += dt * dx.v_lon;
        x.v_lat += dt * dx.v_lat;
        x.d_psi += dt * dx.d_psi;
        x.v = x.v_lon;
        dx.v = dx.v_lon;
    }

    x.delta += dt * dx.delta;
    x.delta = std::min(x.delta, car.limits.max_delta);
    x.delta = std::max(x.delta, -car.limits.max_delta);

    double acc_x = dx.v_lon - x.v_lat * x.d_psi;
    double acc_y = dx.v_lat + x.v_lon * x.d_psi;

    car.modelPose.position.x = x.x2;
    car.modelPose.position.z = x.x1;
    car.modelPose.rotation = glm::angleAxis((float)x.psi, glm::vec3(0, 1, 0));
    car.velocity = glm::vec3(dx.x2, 0, dx.x1);
    car.acceleration = glm::vec3(acc_y, 0, acc_x);
    car.steeringAngle = x.delta;
    car.alphaFront = alpha_front;
    car.alphaRear = alpha_rear;

    // user controls ...

    if (getKey(GLFW_KEY_UP) == GLFW_PRESS) {
        car.vesc.velocity = 1.0;
        car.vesc.steeringAngle = 0;
    }
    if (getKey(GLFW_KEY_DOWN) == GLFW_PRESS) {
        car.vesc.velocity = 0.0;
    }
    if (getKey(GLFW_KEY_LEFT) == GLFW_PRESS) {
        car.vesc.steeringAngle = 0.4;
    }
    if (getKey(GLFW_KEY_RIGHT) == GLFW_PRESS) {
        car.vesc.steeringAngle = -0.4;
    }
}

void CarModule::updateMainCamera(
        Scene::Car::MainCamera& carMainCamera,
        Pose& carModelPose) {

    mainCamera.fov = carMainCamera.fovy;
    mainCamera.aspectRatio = carMainCamera.getAspectRatio();
    mainCamera.pose = carMainCamera.pose.transform(carModelPose);

    if (frameBuffer.width != carMainCamera.imageWidth
            || frameBuffer.height != carMainCamera.imageHeight) {
        frameBuffer.resize(carMainCamera.imageWidth,
                carMainCamera.imageHeight);
    }

    if (bayerFrameBuffer.width != carMainCamera.imageWidth
            || bayerFrameBuffer.height != carMainCamera.imageHeight) {
        bayerFrameBuffer.resize(carMainCamera.imageWidth,
                carMainCamera.imageHeight);
    }
}

void CarModule::updateDepthCamera(
        Scene::Car::DepthCamera& carDepthCamera,
        Pose& carModelPose) {

    depthCamera.fov = carDepthCamera.depthFovy;
    depthCamera.aspectRatio = carDepthCamera.getDepthAspectRatio();
    depthCamera.pose = carDepthCamera.pose.transform(carModelPose);

    if (depthCameraFrameBuffer.width != carDepthCamera.depthImageWidth
            || depthCameraFrameBuffer.height != carDepthCamera.depthImageHeight) {
        depthCameraFrameBuffer.resize(carDepthCamera.depthImageWidth,
                carDepthCamera.depthImageHeight);
    }
}

void CarModule::updateLaserSensors(
        Scene::Car& car,
        ModelStore& modelStore,
        std::vector<std::shared_ptr<Scene::Item>>& items) {
    
    float minDist = 1000.0f;

    glm::vec3 binaryLightSensorWorldPos = car.modelPose.getMatrix() * 
        glm::vec4(car.binaryLightSensor.pose.position, 1);

    std::cout << binaryLightSensorWorldPos << std::endl;

    for (std::shared_ptr<Scene::Item>& it : items) {

        if (glm::length(binaryLightSensorWorldPos - it->pose.position) > 1.0) {
            continue;
        }

        glm::vec4 dir{1, 0, 0, 1};
        dir = car.modelPose.getMatrix() * dir;

        Model& itemModel = modelStore.itemModels[it->type];
        glm::mat4 modelMat = it->pose.getMatrix();

        for (unsigned int i = 2 ; i < itemModel.vertices.size() ; i += 3) {

            glm::vec3 vec0 = {
                itemModel.vertices[i-2].Position.X,
                itemModel.vertices[i-2].Position.Y,
                itemModel.vertices[i-2].Position.Z };
            glm::vec3 vec1 = {
                itemModel.vertices[i-1].Position.X,
                itemModel.vertices[i-1].Position.Y,
                itemModel.vertices[i-1].Position.Z };
            glm::vec3 vec2 = {
                itemModel.vertices[i].Position.X,
                itemModel.vertices[i].Position.Y,
                itemModel.vertices[i].Position.Z };

            vec0 = modelMat * glm::vec4(vec0, 1);
            vec1 = modelMat * glm::vec4(vec1, 1);
            vec2 = modelMat * glm::vec4(vec2, 1);

            glm::vec3 intersectionPos;

            if(glm::intersectLineTriangle(
                        binaryLightSensorWorldPos,
                        glm::vec3(dir),
                        vec0,
                        vec1,
                        vec2,
                        intersectionPos)) {

                std::cout << intersectionPos << std::endl;

                if (intersectionPos.x < 0) {
                    minDist = std::min(
                            minDist,
                            glm::length(-intersectionPos.x));
                }
            }
        }
    }
    
    std::cout << "Laser sensor distance: " << minDist << std::endl;
}

void CarModule::render(GLuint shaderProgramId, Scene::Car& car) {

    carModel.render(shaderProgramId, car.modelPose.getMatrix());
}
