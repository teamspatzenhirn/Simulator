#ifndef INC_2019_ITEMSMODULE_H
#define INC_2019_ITEMSMODULE_H

#include "Scene.h"
#include "helpers/Helpers.h"

class ItemsModule {

    Model obstacleModel{"models/obstacle.obj"};
    Model startLineModel{"models/start_line.obj"};
    Model stopLineModel{"models/stop_line.obj"};
    Model giveWayLineModel{"models/give_way_line.obj"};
    Model crosswalkModel{"models/crosswalk.obj"};
    Model ground30Model{"models/30.obj"};

public:

    ItemsModule();
    ~ItemsModule();

    void render(GLuint shaderProgramId,
            std::vector<std::shared_ptr<Scene::Item>>& items);
};

#endif
