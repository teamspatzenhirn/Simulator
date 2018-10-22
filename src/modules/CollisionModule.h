#ifndef INC_2019_COLLISION_H
#define INC_2019_COLLISION_H

#include <vector>
#include <map>

#define _USE_MATH_DEFINES
#include <cmath>

#define GLM_ENABLE_EXPERIMENTAL 
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/io.hpp>

#include "helpers/Helpers.h"

struct RigidBody {

    Pose* pose;

    glm::vec2 p00;
    glm::vec2 p01;
    glm::vec2 p10;
    glm::vec2 p11;
};

class CollisionModule {

    std::vector<RigidBody> bodies;

    std::map<Pose*, std::vector<RigidBody>> collisions;

    glm::vec2 project(glm::vec2 axis, RigidBody& rb);

    bool overlap(glm::vec2 range0, glm::vec2 range1);

public:

    CollisionModule();

    void add(Pose& pose, Model& model);

    void update();

    std::vector<RigidBody> getCollisions(Pose& pose);
};

#endif
