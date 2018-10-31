#ifndef INC_2019_COMMMODULE_H
#define INC_2019_COMMMODULE_H

#include <errno.h>
#include <cstring>

#include "Scene.h"
#include "helpers/Helpers.h"
#include "sharedmem/shmcomm.h"

class CommModule {

    static constexpr int mainCameraMemId = 428769;
    static constexpr int carMemId = 428770;
    static constexpr int vescMemId = 428771;

    struct MainCameraImage {

        /*
         * Assuming 2064x1544 Bayer 8 (one byte per pixel) as the maximum
         * possible image size otherwise ... welp, not good ...
         */
        unsigned char buffer[3186816];

        int imageWidth;
        int imageHeight;
    };

    struct Car {
        
        double x;
        double y;

        double psi;
        double dPsi;

        double steeringAngle;

        double velX;
        double velY;

        double accX;
        double accY;

        double alphaFront;
        double alphaRear;

        uint64_t time;
    };

    struct Vesc {

        double velocity;
        double steeringAngle;
    };

    int vescFailCounter = 0;

    Capture mainCameraCapture;

    SimulatorSHM::SHMComm<MainCameraImage> txMainCamera; 
    SimulatorSHM::SHMComm<Car> txCar; 
    SimulatorSHM::SHMComm<Vesc> rxVesc; 

    template<typename T>
    void initSharedMemory(SimulatorSHM::SHMComm<T>& mem);

public:

    CommModule();
    ~CommModule();

    void transmitMainCamera(Scene::Car& car, GLuint mainCameraFramebufferId);
    void transmitCar(Scene::Car& car, uint64_t time);
    void receiveVesc(Scene::Car::Vesc& car);
};

#endif
