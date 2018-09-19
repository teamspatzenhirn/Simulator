#ifndef INC_2019_SCENE_H
#define INC_2019_SCENE_H

#include <string>
#include <vector>
#include <fstream>

#include <glm/glm.hpp>

/*
 * In order to make simulation state propagration and retention
 * as simple as possible the complete state of the simulator should
 * be encapsulated in this Scene object.
 *
 * That means, that a Scene could contain, for example, the state of the car,
 * the state of all track elements, the position, scale, rotation of obstacles
 * or simply all other relevant information that should be retained between
 * simulator runs or is used by different module.
 *
 * Note that the Scene may only contain simple datatypes, as the
 * object is written AS-IS into a binary file for saving.
 * Saving objects of more complicated derived classes this way will
 * mess with the objects v-tables and corrupt them when loaded again.
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

    Scene();
    Scene(std::string path);

    ~Scene();

    void save(std::string path);
};

#endif
