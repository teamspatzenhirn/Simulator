#include "CommModule.h"

CommModule::CommModule() :
    txMainCamera(SimulatorSHM::SERVER, mainCameraMemId),
    txDepthCamera(SimulatorSHM::SERVER, depthCameraMemId),
    txCar(SimulatorSHM::SERVER, carMemId),
    rxVesc(SimulatorSHM::CLIENT, vescMemId) { 

    initSharedMemory(txMainCamera);
    initSharedMemory(txDepthCamera);
    initSharedMemory(rxVesc);
    initSharedMemory(txCar);
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

void CommModule::transmitMainCamera(Scene::Car& car, GLuint mainCameraFramebufferId) {

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

void CommModule::transmitDepthCamera(Scene::Car& car, GLuint depthCameraFramebufferId) {

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

void CommModule::transmitCar(Scene::Car& car, bool paused, uint64_t time) {

    Car* obj = txCar.lock(SimulatorSHM::WRITE_OVERWRITE_OLDEST); 

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
        obj->time = time;

        /*
         * TODO: sucks
         */
        for (KeyEvent& e : getKeyEvents()) {
            if (e.key == GLFW_KEY_R && e.action == GLFW_PRESS) {
                obj->paused = true;
            }
        }

        txCar.unlock(obj);
    }
}

void CommModule::receiveVesc(Scene::Car::Vesc& vesc) {

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
