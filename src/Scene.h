#ifndef INC_2019_SCENE_H
#define INC_2019_SCENE_H

#include <string>
#include <vector>
#include <fstream>

#include <glm/glm.hpp>

#include <helpers/Helpers.h>

/*
 * In order to make simulation state propagration and retention
 * as simple as possible the complete simulation state should be
 * encapsulated in this Scene object.
 *
 * That means, that a Scene could contain, for example, the state of the car,
 * the state of all track elements, the position, scale, rotation of obstacles
 * or simply all other relevant information that should be retained between
 * simulator runs or is used by different module.
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

        Pose pose{0, 1.0f, 2.0f};

        double delta = 0;
        double v = 0;
        double v_lon = 0;
        double v_lat = 0; 
        double d_psi = 0;     

        struct MainCamera {

            Pose pose{0, 0, 0};

            unsigned int imageWidth = 800;
            unsigned int imageHeight = 600;

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
    Scene(std::string path);

    ~Scene();

    void save(std::string path);
};

#endif
