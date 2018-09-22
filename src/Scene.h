#ifndef INC_2019_SCENE_H
#define INC_2019_SCENE_H

#include <string>
#include <vector>
#include <fstream>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

#include <helpers/Helpers.h>

/*
 * In order to make simulation state propagration and retention
 * as simple as possible the complete simulation state should be
 * encapsulated in this Scene object.
 *
 * That means, that a Scene could contain, for example, the state of the car,
 * the state of all track elements, the position, scale, rotation of obstacles
 * or simply all other relevant information that should be retained between
 * simulator runs or is used by different modules.
 */
struct Scene {

    /*
     * This is the current most recent version of the Scene object.
     * Increment this, whenever there were changes made to Scene.
     */
    static const unsigned int VERSION = 0;

    /*
     * This is the actual version of the scene object.
     */
    unsigned int version = 0;

    struct Car {

        Pose modelPose{0.0, 0.0, 0.0};

        glm::vec3 velocity{0, 0, 0};
        glm::vec3 acceleration{0, 0, 0};

        double steerAngle = 0;

        double alphaFront = 0;
        double alphaRear = 0;

        struct SystemParams {

            // radabstand (m)
            double axesDistance = 0.225; 

            // Verhältnis des Moments vorder/hinter Achse;
            double axesMomentRatio = 0.5; 
            
            // kg*m^2 
            double inertia = 0.042; 

            // masse (kg)
            double mass = 3.875; 

            // center of gravity to front axle
            double distCogToFrontAxle = axesDistance / 2.0;

            // center of gravity to rear axle
            double distCogToRearAxle = axesDistance - distCogToFrontAxle;
           
            double getM () {

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
            double delta = 0;
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

            double velocity = 1.5;
            double steeringAngle = 0.1;

        } vesc;

        /*
         * This contains important paramters of the main
         * color camera of the vehicle.
         */
        struct MainCamera {

            Pose pose{0, 0, 0};

            unsigned int imageWidth = 2064;
            unsigned int imageHeight = 1544;

            float fovy = M_PI * 0.5f;

            struct DistortionCoefficients {

                double radial[3];
                double tangential[3];

            } distortionCoefficients;

            float getAspectRatio() {
                return (float)imageWidth / (float)imageHeight;
            }

        } mainCamera;

    } car;

    Scene();
    ~Scene();

    bool save(std::string path);
    bool load(std::string path);
};

#endif
