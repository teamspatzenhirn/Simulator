#ifndef INC_2019_COMMMODULE_H
#define INC_2019_COMMMODULE_H

#include <errno.h>
#include <cstring>

#include "scene/Scene.h"
#include "helpers/Capture.h"
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
        unsigned char buffer[2064*1544];

        int imageWidth;
        int imageHeight;
    };

    struct DepthCameraImage {

        /*
         * Here we assume 640x480 to be the maximum possible depth image size of one camera.
         */
        float buffer[640*2*480*3];

        int imageWidth;
        int imageHeight;
    };

    struct CarState {
        
        double x;
        double y;

        double psi;
        double dPsi;

        double steeringAngleFront;
        double steeringAngleRear;

        double velX;
        double velY;

        double accX;
        double accY;

        double alphaFront;
        double alphaRear;

        double time;

        double drivenDistance;

        bool paused;

        float laserSensorValue;

        bool binaryLightSensorTriggered;
    };

    struct Vesc {

        double velocity;
        double steeringAngleFront, steeringAngleRear;
    };

    struct Visualization {

        glm::vec2 trajectoryPoints[128];
    };

    int vescFailCounter = 0;

    SimulatorSHM::SHMComm<MainCameraImage> txMainCamera; 
    SimulatorSHM::SHMComm<DepthCameraImage> txDepthCamera; 
    SimulatorSHM::SHMComm<CarState> txCarState; 
    SimulatorSHM::SHMComm<Vesc> rxVesc; 
    SimulatorSHM::SHMComm<Visualization> rxVisual; 

    template<typename T>
    void initSharedMemory(SimulatorSHM::SHMComm<T>& mem);

public:

    CommModule();
    ~CommModule();

    void transmitMainCamera(
            Car& car, 
            Capture& mainCameraCapture, 
            GLuint mainCameraFramebufferId);

    void transmitDepthCamera(
            Car& car, 
            Capture& depthCameraCapture, 
            GLuint depthCameraFramebufferId);

    void transmitCar(Car& car, bool paused, double simulationTime);
    void receiveVesc(Car::Vesc& car);
    void receiveVisualization(Scene::Visualization& vis);
};

#endif
