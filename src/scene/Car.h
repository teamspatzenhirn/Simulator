#ifndef INC_2019_CAR_H
#define INC_2019_CAR_H

#include "helpers/Id.h"
#include "helpers/Pose.h"

/*
 * This struct contains the state of the simulated model car.
 */
struct Car {

    uint64_t id = getId();

    Pose modelPose{0.0, 0.0, 0.0};

    glm::vec3 velocity{0, 0, 0};
    glm::vec3 acceleration{0, 0, 0};

    double steeringAngleFront, steeringAngleRear = 0;

    double alphaFront = 0;
    double alphaRear = 0;

    double drivenDistance = 0;

    struct SystemParams {

        // radabstand (m)
        double axesDistance = 0.2565;

        // Verhältnis des Moments vorder/hinter Achse;
        double axesMomentRatio = 0.5;

        // masse (kg)
        double mass = 5.602;

        // kg*m^2, rectangular vehicle: (1/12) * mass * (width²+length²)
        double inertia = (1.0 / 12.0) * mass * (0.18 * 0.18 + 0.37 * 0.37);

        // center of gravity to front axle
        double distCogToFrontAxle = axesDistance / 2.0;

        // center of gravity to rear axle
        double distCogToRearAxle = axesDistance - distCogToFrontAxle;

        double getM() {

            return (mass
                    * distCogToRearAxle
                    * distCogToRearAxle
                    + inertia)
                   / axesDistance
                   / axesDistance;
        }

    } systemParams;

    struct SimulatorState {

        double x1 = 0;
        double x2 = 0;
        double psi = 0;
        double deltaFront, deltaRear = 0;
        double v = 0;
        double v_lon = 0;
        double v_lat = 0;
        double d_psi = 0;

    } simulatorState;

    struct Limits {

        double max_F = 5;
        double max_delta = 0.35;
        double max_d_delta = 4;

    } limits;

    struct Wheels {

        bool usePacejkaModel = true;

        double B_front = 1;
        double B_rear = 1;
        double C_front = 1.9;
        double C_rear = 1.9;
        double D_front = 12;
        double D_rear = 12;
        double k_front = 10;
        double k_rear = 10;

    } wheels;

    /*
     * These are the parameters that are send to the VESC motor
     * controller.
     */
    struct Vesc {

        double velocity = 0;
        double steeringAngleFront, steeringAngleRear = 0;

    } vesc;

    /*
     * This contains important parameters of the main
     * color camera of the vehicle.
     */
    struct MainCamera {

        // Approximate results from Spatz11 camera calib
        Pose pose{0.0f, 0.3f, 0.15f};

        MainCamera() {
            // Same angle of 10° as Spatz11 IRL
            pose.setEulerAngles(glm::vec3(-10.00f, 180.0f, 0.0f));
        }

        /*
         * Interpolation factor, between an image consisting only
         * of noise and the camera image without any noise.
         * Noise calculation done in shader.
         */
        float noise = 0.2f;

        int imageWidth = 2048;
        int imageHeight = 1536;

        // FOV height, adjusted by varying the parameter until the image looked like an undistorted camera image
        float fovy = 1.7;

        struct DistortionCoefficients {

            float radial[3] = {0, 0, 0};
            float tangential[3] = {0, 0, 0};

        } distortionCoefficients;

        float getAspectRatio() {
            return (float) imageWidth / (float) imageHeight;
        }

    } mainCamera;

    /*
     * This contains the parameters for the color and the depth images
     * produced by the depth camera.
     */
    struct DepthCamera {

        Pose pose{0, 0.19f, 0.05f};

        DepthCamera() {
            pose.setEulerAngles(glm::vec3(0.0f, 180.0f, 0.0f));
        }

        int colorImageWidth = 1280;
        int colorImageHeight = 720;

        int depthImageWidth = 640 * 2;
        int depthImageHeight = 480;

        float colorFovy = (float) M_PI * 0.5f;
        float depthFovy = (float) M_PI * 0.25f;

        float getColorAspectRatio() {
            return (float) colorImageWidth / (float) colorImageHeight;
        }

        float getDepthAspectRatio() {
            return (float) depthImageWidth / (float) depthImageHeight;
        }

    } depthCamera;

    struct BinaryLightSensor {

        /*
         * The position of the sensor in car coordinate.
         */
        Pose pose{/*left*/ -0.084f, /*up*/ 0.085f, /*front*/ -0.034f};

        /*
         * The distance detected by this light sensors
         */
        float value = 100.0f;

        /*
         * Whether the sensor is triggered or not.
         * Updated in SensorModule.
         */
        bool triggered = false;

        /*
         * The minimum trigger distance to an obstacle in meters.
         */
        float triggerDistance = 0.3f;

    } binaryLightSensor;

    struct LaserSensor {

        /*
         * The position of the sensor in car coordinate.
         */
        Pose pose{-0.05f, 0.1f, 0.21f};

        /*
         * The distance detected by this light sensors
         */
        float value = 1000.0f;

    } laserSensor;
};

#endif

