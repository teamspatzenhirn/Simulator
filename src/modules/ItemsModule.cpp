#include "ItemsModule.h"

ItemsModule::ItemsModule() { 

}

ItemsModule::~ItemsModule() {

}

void ItemsModule::updateDynamicObstacles(
        float dt,
        Scene::Car& car, 
        std::vector<std::shared_ptr<Scene::Item>>& items) {
    
    for (std::shared_ptr<Scene::Item>& i : items) {
        if (DYNAMIC_OBSTACLE == i->type) {

            if (obstStates.find(i.get()) == obstStates.end()) {
                obstStates[i.get()] = { i->pose, false };
            }

            DynamicObstacleState& state = obstStates[i.get()];

            float dist = glm::length(
                    car.modelPose.position - i->pose.position);

            if (dist <= 2 && car.vesc.velocity > 0.01f && !state.active) {
                state.active = true;
                state.startPose = i->pose;
            } 

            if (state.active == true) {
                i->pose.position = glm::vec3(
                        i->pose.getMatrix() * glm::vec4(0, 0, 0.0006 * dt, 1));
                if (dist > 2) {
                    state.active = false;
                    i->pose = state.startPose;
                }
            }
        }
    }
}

void ItemsModule::update(
        std::vector<std::shared_ptr<Scene::Item>>& items,
        Pose* selection) {

    if (nullptr == selection) {
        return;
    }

    bool del = false;
    for (KeyEvent& e : getKeyEvents()) {
        if ((e.key == GLFW_KEY_BACKSPACE && e.action == GLFW_PRESS)
                || (e.key == GLFW_KEY_DELETE && e.action == GLFW_PRESS)) {
            del = true;
        }
    }

    if (!del) {
        return;
    }

    items.erase(
            std::remove_if(
                items.begin(),
                items.end(),
                [&](const std::shared_ptr<Scene::Item>& i){
                    return &i.get()->pose == selection;
                }),
            items.end());
}

void ItemsModule::render(
        GLuint shaderProgramId,
        ModelStore& modelStore, 
        std::vector<std::shared_ptr<Scene::Item>>& items) {
    
    for (std::shared_ptr<Scene::Item>& i : items) {
        glm::mat4 modelMat = i->pose.getMatrix();
        modelStore.itemModels[i->type].render(shaderProgramId, modelMat);
    }
}
