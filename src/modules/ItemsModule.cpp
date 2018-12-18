#include "ItemsModule.h"

ItemsModule::ItemsModule() { 

}

ItemsModule::~ItemsModule() {

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
