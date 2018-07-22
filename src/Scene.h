#ifndef INC_2019_SCENE_H
#define INC_2019_SCENE_H

#include <string>
#include <vector>
#include <glm/glm.hpp>

class Scene {

public:

    Scene();
    Scene(std::string path);

    ~Scene();

    void render();
};

#endif