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
    static constexpr int depthCameraMemId = 428772;
    static constexpr int visualMemId = 428773;

    struct MainCameraImage {

        /*
         * Assuming 2064x1544 Bayer 8 (one byte per pixel) as the maximum
         * possible image size otherwise ... welp, not good ...
         */
        unsigned char buffer[3186816];

        int imageWidth;
        int imageHeight;
    };

    struct DepthCameraImage {

        /*
         * Here we assume 640x480 to be the maximum possible depth image size.
         */
        float buffer[640*480*3];

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

        double time;

        bool paused;

        float laserSensorValue;

        bool binaryLightSensorTriggered;
    };

    struct Vesc {

        double velocity;
        double steeringAngle;
    };

    struct Visualization {

        glm::vec2 trajectoryPoints[128];
    };

    int vescFailCounter = 0;

    Capture mainCameraCapture;
    Capture depthCameraCapture;

    DepthCameraImage* lastDepthObj = nullptr;

    SimulatorSHM::SHMComm<MainCameraImage> txMainCamera; 
    SimulatorSHM::SHMComm<DepthCameraImage> txDepthCamera; 
    SimulatorSHM::SHMComm<Car> txCar; 
    SimulatorSHM::SHMComm<Vesc> rxVesc; 
    SimulatorSHM::SHMComm<Visualization> rxVisual; 

    template<typename T>
    void initSharedMemory(SimulatorSHM::SHMComm<T>& mem);

public:

    CommModule();
    ~CommModule();

    void transmitMainCamera(Scene::Car& car, GLuint mainCameraFramebufferId);
    void transmitDepthCamera(Scene::Car& car, GLuint depthCameraFramebufferId);
    void transmitCar(Scene::Car& car, bool paused, double simulationTime);
    void receiveVesc(Scene::Car::Vesc& car);
    void receiveVisualization(Scene::Visualization& vis);
};

#endif
