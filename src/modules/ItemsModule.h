#ifndef INC_2019_ITEMSMODULE_H
#define INC_2019_ITEMSMODULE_H

#include <map>
#include <algorithm>

#include "scene/ModelStore.h"

#include "scene/Scene.h"
#include "helpers/Helpers.h"

class ItemsModule {

    struct DynamicItemState {
        Pose startPose;
        bool active;
    };

    std::map<uint64_t, DynamicItemState> itemState;

public:

    ItemsModule();
    ~ItemsModule();

    void updateDynamicItems(
            float dt,
            Car& car, 
            Scene::DynamicItemSettings& dis,
            std::vector<Scene::Item>& items);

    void update(
            std::vector<Scene::Item>& items,
            Pose* selection);

    void render(
            GLuint shaderProgramId,
            ModelStore& modelStore,
            std::vector<Scene::Item>& items);
};

#endif
