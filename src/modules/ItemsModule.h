#ifndef INC_2019_ITEMSMODULE_H
#define INC_2019_ITEMSMODULE_H

#include "Scene.h"
#include "helpers/Helpers.h"

class ItemsModule {

    struct DynamicObstacleState {
        Pose startPose;
        bool active;
    };

    std::map<Scene::Item*, DynamicObstacleState> obstStates;

public:

    ItemsModule();
    ~ItemsModule();

    void updateDynamicObstacles(
            float dt,
            Scene::Car& car, 
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
