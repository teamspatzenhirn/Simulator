#include "CarModule.h"

CarModule::CarModule() {

}

void CarModule::update(Scene::Car& car) {

    mainCamera.fov = car.mainCamera.fovy;
    mainCamera.aspectRatio = car.mainCamera.getAspectRatio();

    if (frameBuffer.width != car.mainCamera.imageWidth
            || frameBuffer.height != car.mainCamera.imageHeight) {
        frameBuffer.resize(car.mainCamera.imageWidth,
                car.mainCamera.imageHeight);
    }
}
