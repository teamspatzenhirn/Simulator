#ifndef INC_2019_CARMODULE_H
#define INC_2019_CARMODULE_H

#include <glm/glm.hpp>

#include "helpers/Helpers.h"

class CarModule {

public:

    glm::vec3 position;

    Camera camera;
    FrameBuffer frameBuffer{2064, 1544};

    CarModule(glm::vec3 position, float fovy, float cameraAspect);

};

#endif
