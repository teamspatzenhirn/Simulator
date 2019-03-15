#ifndef INC_2019_ITEMSMODULE_H
#define INC_2019_ITEMSMODULE_H

#include <map>
#include <algorithm>

#include "Scene.h"
#include "helpers/Helpers.h"

class ItemsModule {

    struct DynamicItemState {
        Pose startPose;
        bool active;
    };

    std::map<Scene::Item*, DynamicItemState> itemState;

public:

    ItemsModule();
    ~ItemsModule();

    void updateDynamicItems(
            float dt,
            Car& car, 
            Scene::DynamicItemSettings& dis,
            std::vector<std::shared_ptr<Scene::Item>>& items);

    void update(
            std::vector<std::shared_ptr<Scene::Item>>& items,
            Pose* selection);

    void render(
            GLuint shaderProgramId,
            ModelStore& modelStore,
            std::vector<std::shared_ptr<Scene::Item>>& items);
};

#endif
