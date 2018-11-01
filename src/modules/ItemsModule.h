#ifndef INC_2019_ITEMSMODULE_H
#define INC_2019_ITEMSMODULE_H

#include "Scene.h"
#include "helpers/Helpers.h"

class ItemsModule {

public:

    Model obstacleModel{"models/obstacle.obj"};
    Model startLineModel{"models/start_line.obj"};
    Model stopLineModel{"models/stop_line.obj"};
    Model giveWayLineModel{"models/give_way_line.obj"};
    Model crosswalkModel{"models/crosswalk.obj"};
    Model ground10Model{"models/ground_10.obj"};
    Model ground20Model{"models/ground_20.obj"};
    Model ground30Model{"models/ground_30.obj"};
    Model ground40Model{"models/ground_40.obj"};
    Model ground50Model{"models/ground_50.obj"};
    Model ground60Model{"models/ground_60.obj"};
    Model ground70Model{"models/ground_70.obj"};
    Model ground80Model{"models/ground_80.obj"};
    Model ground90Model{"models/ground_90.obj"};
    Model ground10EndModel{"models/ground_10_end.obj"};
    Model ground20EndModel{"models/ground_20_end.obj"};
    Model ground30EndModel{"models/ground_30_end.obj"};
    Model ground40EndModel{"models/ground_40_end.obj"};
    Model ground50EndModel{"models/ground_50_end.obj"};
    Model ground60EndModel{"models/ground_60_end.obj"};
    Model ground70EndModel{"models/ground_70_end.obj"};
    Model ground80EndModel{"models/ground_80_end.obj"};
    Model ground90EndModel{"models/ground_90_end.obj"};
    Model groundArrowLeftModel{"models/ground_arrow_left.obj"};
    Model groundArrowRightModel{"models/ground_arrow_right.obj"};
    Model endModel{"models/end.obj"};

    ItemsModule();
    ~ItemsModule();

    void update(std::vector<std::shared_ptr<Scene::Item>>& items,
            Pose* selection);

    void render(GLuint shaderProgramId,
            std::vector<std::shared_ptr<Scene::Item>>& items);
};

#endif
