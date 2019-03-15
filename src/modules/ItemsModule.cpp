#include "ItemsModule.h"

ItemsModule::ItemsModule() { 

}

ItemsModule::~ItemsModule() {

}

void ItemsModule::updateDynamicItems(
        float dt,
        Car& car, 
        Scene::DynamicItemSettings& dis,
        std::vector<std::shared_ptr<Scene::Item>>& items) {
    
    for (std::shared_ptr<Scene::Item>& i : items) {

        float dist = glm::length(
                car.modelPose.position - i->pose.position);

        if (itemState.find(i.get()) == itemState.end()) {
            itemState[i.get()] = { i->pose, false };
        }

        DynamicItemState& state = itemState[i.get()];

        if (DYNAMIC_OBSTACLE == i->type) {

            if (dist <= 2 && car.vesc.velocity > 0.01f && !state.active) {
                state.active = true;
                state.startPose = i->pose;
            } 

            if (state.active == true) {
                glm::vec3 newPos = glm::vec3(
                        i->pose.getMatrix() * glm::vec4(0, 0, dis.speed * dt / 1000.0, 1));

                i->pose.position = newPos;

                if (dist > 2) {
                    state.active = false;
                    i->pose = state.startPose;
                }
            }
        }

        if (DYNAMIC_PEDESTRIAN_RIGHT == i->type) {

            if (dist <= 1 
                    && car.vesc.velocity < 0.01f
                    && car.acceleration.z < 0
                    && !state.active) {
                state.active = true;
                state.startPose = i->pose;
            } 

            if (state.active == true) {
                i->pose.position = glm::vec3(
                        i->pose.getMatrix() * glm::vec4(0.00015 * dt, 0, 0, 1));
                if (dist > 2) {
                    state.active = false;
                    i->pose = state.startPose;
                }
            }
        }

        if (DYNAMIC_PEDESTRIAN_LEFT == i->type) {

            if (dist <= 1 
                    && car.vesc.velocity < 0.01f
                    && car.acceleration.z < 0
                    && !state.active) {
                state.active = true;
                state.startPose = i->pose;
            } 

            if (state.active == true) {
                i->pose.position = glm::vec3(
                        i->pose.getMatrix() * glm::vec4(-0.00015 * dt, 0, 0, 1));
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
