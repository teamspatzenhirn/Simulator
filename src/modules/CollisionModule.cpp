#include "modules/CollisionModule.h"

CollisionModule::CollisionModule() {
}

void CollisionModule::add(Pose& pose, Model& model) {

    /*
     * Projects all of the rigid bodies on the XZ ground plane
     * and store the result.
     *
     * Let the bodies hit the floor!
     * Let the bodies hit the floor!
     * Let the bodies hit the flooooooor!
     */
 
    glm::vec3 h0 = model.boundingBox.center - model.boundingBox.size / 2.0f;
    glm::vec3 h1 = model.boundingBox.center + model.boundingBox.size / 2.0f;

    std::vector<glm::vec4> points = {
        glm::vec4(h0.x, h0.y, h0.z, 1),
        glm::vec4(h0.x, h0.y, h1.z, 1),
        glm::vec4(h1.x, h0.y, h0.z, 1),
        glm::vec4(h1.x, h0.y, h1.z, 1),
    };

    glm::mat4 modelMat = pose.getMatrix();

    for (unsigned int i = 0; i < points.size(); ++i) {
        points[i] = modelMat * points[i];
    }

    bodies.emplace_back();
    RigidBody& rb = bodies.back();
    
    rb.pose = &pose;
    rb.p00 = {points[0].x, points[0].z};
    rb.p01 = {points[1].x, points[1].z};
    rb.p10 = {points[2].x, points[2].z};
    rb.p11 = {points[3].x, points[3].z};
}

void CollisionModule::update() {

    /*
     * Using separating axis theorem to check for collisions
     * between all rigid bodies. 
     */

    collisions.clear();

    for (size_t i = 0; i < bodies.size(); ++i) {

        RigidBody body = bodies[i];

        for (size_t j = 0; j < i; ++j) {

            RigidBody other = bodies[j];

            glm::vec2 axis0 = body.p01 - body.p00;
            glm::vec2 axis1 = body.p10 - body.p00;
            glm::vec2 axis2 = other.p01 - other.p00;
            glm::vec2 axis3 = other.p10 - other.p00;

            if (!overlap(project(axis0, body), project(axis0, other))) {
                continue;
            }
            if (!overlap(project(axis1, body), project(axis1, other))) {
                continue;
            }
            if (!overlap(project(axis2, body), project(axis2, other))) {
                continue;
            }
            if (!overlap(project(axis3, body), project(axis3, other))) {
                continue;
            }

            collisions[body.pose].push_back(body);
            collisions[other.pose].push_back(other);
        }

    }

    bodies.clear();
}

glm::vec2 CollisionModule::project(glm::vec2 axis, RigidBody& rb) {

    float r0 = glm::dot(axis, rb.p00);
    float r1 = glm::dot(axis, rb.p01);
    float r2 = glm::dot(axis, rb.p10);
    float r3 = glm::dot(axis, rb.p11);

    float rMax = std::max(r3, std::max(r2, std::max(r1, r0)));
    float rMin = std::min(r3, std::min(r2, std::min(r1, r0)));

    return glm::vec2(rMin, rMax);
}

bool CollisionModule::overlap(glm::vec2 range0, glm::vec2 range1) {

    return range0.y > range1.x && range0.x < range1.y;
}

std::vector<RigidBody> CollisionModule::getCollisions(Pose& pose) {

    if (collisions.end() == collisions.find(&pose)) {
        return {};
    } else {
        return collisions[&pose];
    }
}
