#include "CommModule.h"

CommModule::CommModule() :
    txMainCamera(SimulatorSHM::SERVER, mainCameraMemId),
    txDepthCamera(SimulatorSHM::SERVER, depthCameraMemId),
    txCarState(SimulatorSHM::SERVER, carMemId),
    rxVesc(SimulatorSHM::CLIENT, vescMemId),
    rxVisual(SimulatorSHM::CLIENT, visualMemId) { 

    initSharedMemory(txMainCamera);
    initSharedMemory(txDepthCamera);
    initSharedMemory(txCarState);
    initSharedMemory(rxVesc);
    initSharedMemory(rxVisual);
}

CommModule::~CommModule() {

}

template<typename T>
void CommModule::initSharedMemory(SimulatorSHM::SHMComm<T>& mem) {

    if(!mem.attach()) {
        std::cout << "Shared memory init failed!" << std::endl;
        std::exit(-1);
    }
}

void CommModule::transmitMainCamera(Car& car, GLuint mainCameraFramebufferId) {

    glBindFramebuffer(GL_FRAMEBUFFER, mainCameraFramebufferId);

    // download image from opengl to shared memory buffer

    MainCameraImage* obj = txMainCamera.lock(SimulatorSHM::WRITE_OVERWRITE_OLDEST); 

    if (obj != nullptr) {

        obj->imageWidth = car.mainCamera.imageWidth;
        obj->imageHeight = car.mainCamera.imageHeight;

        mainCameraCapture.capture(
                (GLubyte*)obj->buffer,
                car.mainCamera.imageWidth,
                car.mainCamera.imageHeight,
                1,
                GL_RED,
                GL_UNSIGNED_BYTE);

        txMainCamera.unlock(obj);
    } 

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CommModule::transmitDepthCamera(Car& car, GLuint depthCameraFramebufferId) {

    glBindFramebuffer(GL_FRAMEBUFFER, depthCameraFramebufferId);

    // download image from opengl to shared memory buffer

    DepthCameraImage* obj = txDepthCamera.lock(SimulatorSHM::WRITE_OVERWRITE_OLDEST); 

    if (obj != nullptr) {

        obj->imageWidth = car.depthCamera.depthImageWidth;
        obj->imageHeight = car.depthCamera.depthImageHeight;

        depthCameraCapture.capture(
                (GLubyte*)obj->buffer,
                car.depthCamera.depthImageWidth,
                car.depthCamera.depthImageHeight,
                4 * 3,
                GL_RGB,
                GL_FLOAT);

        txDepthCamera.unlock(obj);
    } 

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CommModule::transmitCar(Car& car, bool paused, double simulationTime) {

    CarState* obj = txCarState.lock(SimulatorSHM::WRITE_OVERWRITE_OLDEST); 

    if (obj != nullptr) {

        obj->x = car.simulatorState.x1;
        obj->y = car.simulatorState.x2;
        obj->psi = car.simulatorState.psi;
        obj->dPsi = car.simulatorState.d_psi;
        obj->steeringAngle = car.steeringAngle;
        obj->velX = car.velocity.z;
        obj->velY = car.velocity.x;
        obj->accX = car.acceleration.z;
        obj->accY = car.acceleration.x;
        obj->alphaFront = car.alphaFront;
        obj->alphaRear = car.alphaRear;
        obj->time = simulationTime;
        obj->laserSensorValue = car.laserSensor.value;
        obj->binaryLightSensorTriggered = car.binaryLightSensor.triggered;
        obj->paused = paused;

        /*
         * TODO: sucks
         */
        for (KeyEvent& e : getKeyEvents()) {
            if (e.key == GLFW_KEY_R && e.action == GLFW_PRESS) {
                obj->paused = true;
            }
        }

        txCarState.unlock(obj);
    }
}

void CommModule::receiveVesc(Car::Vesc& vesc) {

    Vesc* obj = rxVesc.lock(SimulatorSHM::READ_NEWEST);

    if (obj != nullptr) {
        vescFailCounter = 0;

        vesc.velocity = obj->velocity;
        vesc.steeringAngle = obj->steeringAngle;

        rxVesc.unlock(obj);
    } else if (vescFailCounter == 100) {
        vesc.velocity = 0.0f;
        vesc.steeringAngle = 0.0f;
        vescFailCounter++;
    } else {
        vescFailCounter++;
    }
}

void CommModule::receiveVisualization(Scene::Visualization& vis) {

    Visualization* obj = rxVisual.lock(SimulatorSHM::READ_NEWEST);

    if (obj != nullptr) {

        vis.trajectoryPoints.clear();

        for (int i = 0; i < 128; i++) {
            glm::vec2 pos = obj->trajectoryPoints[i];
            vis.trajectoryPoints.emplace_back(pos.y, pos.x);
        }

        rxVisual.unlock(obj);
    } 
}
