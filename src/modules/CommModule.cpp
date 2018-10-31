#include "CommModule.h"

CommModule::CommModule() :
    txMainCamera(SimulatorSHM::SERVER, mainCameraMemId),
    txCar(SimulatorSHM::SERVER, carMemId),
    rxVesc(SimulatorSHM::CLIENT, vescMemId) { 

    initSharedMemory(txMainCamera);
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
                GL_COLOR_ATTACHMENT0);

        txMainCamera.unlock(obj);
    } 

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CommModule::transmitCar(Scene::Car& car, uint64_t time) {

    Car* obj = txCar.lock(SimulatorSHM::WRITE_OVERWRITE_OLDEST); 

    if (obj != nullptr) {
        /*
         * Why is it necessary to negate the y-axis, psi, dPsi and
         * the steeringAngle here?! Does not make any sence!
         */
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
