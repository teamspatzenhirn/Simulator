/**
 * @file CinematicCamera.h
 * @author jonas
 * @date 10/22/19
 * Camera that is following the car with smooth movement
 */

#ifndef SPATZSIM_CINEMATICCAMERA_H
#define SPATZSIM_CINEMATICCAMERA_H

#include "Camera.h"

class CinematicCamera : public Camera {

public:

    void update(Pose &carPose);
};

#endif //SPATZSIM_CINEMATICCAMERA_H
