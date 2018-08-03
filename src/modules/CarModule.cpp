#include "CarModule.h"

CarModule::CarModule(glm::vec3 position, float fovy, float cameraAspect)
    : position(position), camera{fovy, cameraAspect} {

    camera.view = glm::translate(camera.view, -position);
}
