#include "CommModule.h"

CommModule::CommModule() :
    txMainCamera(SimulatorSHM::SERVER, mainCameraMemId),
    txCar(SimulatorSHM::SERVER, carMemId),
    rxVesc(SimulatorSHM::CLIENT, vescMemId) { 

    initSharedMemory(txMainCamera);
    initSharedMemory(txCar);

    mainCameraIntermediateBuffer = nullptr;
    mainCameraIntermediateBufferSize = -1;
}

CommModule::~CommModule() {

    delete[] mainCameraIntermediateBuffer;
}

template<typename T>
void CommModule::initSharedMemory(SimulatorSHM::SHMComm<T>& mem) {

    if(!mem.attach()) {
        mem.destroy();
        if(!mem.attach()) {
            std::cout << "Shared memory init failed!" << std::endl;
            std::exit(-1);
        }
    }
}

void CommModule::transmitMainCamera(Scene::Car& car, GLuint mainCameraFramebufferId) {

    glBindFramebuffer(GL_FRAMEBUFFER, mainCameraFramebufferId);

    // download image from opengl to shared memory buffer

    MainCameraImage* obj = txMainCamera.lock(SimulatorSHM::WRITE_NO_OVERWRITE); 

    if (obj != nullptr) {

        unsigned int dataSize =
            car.mainCamera.imageWidth * car.mainCamera.imageHeight * 3;

        // TODO: tu moch capy ...

        if (mainCameraIntermediateBufferSize != dataSize) {
            delete[] mainCameraIntermediateBuffer;
            mainCameraIntermediateBuffer = new GLubyte[dataSize];
            mainCameraIntermediateBufferSize = dataSize;
        }

        mainCameraCapture.capture(
                mainCameraIntermediateBuffer,
                car.mainCamera.imageWidth,
                car.mainCamera.imageHeight,
                GL_COLOR_ATTACHMENT0);

        obj->imageWidth = car.mainCamera.imageWidth;
        obj->imageHeight = car.mainCamera.imageHeight;

        // conversion to bayer pattern

        const unsigned char* source = mainCameraIntermediateBuffer;
        unsigned char* dest = obj->buffer;

        int srcRow;
        int row;

        for(unsigned int y = 0; y < car.mainCamera.imageHeight; y++){
            srcRow = (car.mainCamera.imageHeight - y - 1) * car.mainCamera.imageWidth;
            row = y * car.mainCamera.imageWidth;
            for(unsigned int x = 0; x < car.mainCamera.imageWidth; x++){
                dest[row+x] = source[((srcRow+x)*3)+x%2]; // BGBGBGBG...
            }
            y++;
            srcRow = (car.mainCamera.imageHeight - y - 1) * car.mainCamera.imageWidth;
            row = y * car.mainCamera.imageWidth;
            for(unsigned int x = 0; x < car.mainCamera.imageWidth; x++){
                dest[row+x] = source[((srcRow+x)*3)+1+x%2]; // GRGRGRGRGR...
            }
        }

        txMainCamera.unlock(obj);
    } 

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CommModule::transmitCar(Scene::Car& car) {

    Car* obj = txCar.lock(SimulatorSHM::WRITE_NO_OVERWRITE); 

    if (obj != nullptr) {
        /*
         * Why is it necessary to negate the y-axis, psi, dPsi and
         * the steeringAngle here?! Does not make any sense!
         */
        obj->x = car.simulatorState.x1;
        obj->y = -car.simulatorState.x2;
        obj->psi = -car.simulatorState.psi;
        obj->dPsi = -car.simulatorState.d_psi;
        obj->steeringAngle = -car.steeringAngle;
        obj->velX = car.velocity.z;
        obj->velY = -car.velocity.x;
        obj->accX = car.acceleration.z;
        obj->accY = -car.acceleration.x;
        obj->alphaFront = car.alphaFront;
        obj->alphaRear = car.alphaRear;

        txCar.unlock(obj);
    }
}

void CommModule::receiveVesc(Scene::Car::Vesc& vesc) {

    if (!rxVesc.ok()) {
        rxVesc = SimulatorSHM::SHMComm<Vesc>(SimulatorSHM::CLIENT, vescMemId);
        rxVesc.attach();
        //std::cout << std::strerror(errno) << std::endl;
    }

    if (rxVesc.ok()) {

        Vesc* obj = rxVesc.lock(SimulatorSHM::READ_OLDEST);

        if (obj != nullptr) {

            vesc.velocity = obj->velocity;
            vesc.steeringAngle = -obj->steeringAngle;

            rxVesc.unlock(obj);
        }
    } else {
        vesc.velocity = 0.0f;
        vesc.steeringAngle = 0.0f;
    }
}
