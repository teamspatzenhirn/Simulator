#ifndef INC_2019_ITEMSMODULE_H
#define INC_2019_ITEMSMODULE_H

#include "Scene.h"
#include "helpers/Helpers.h"

class ItemsModule {

public:

    Model models[28] = {
        Model{},
        Model{"models/obstacle.obj"},
        Model{"models/start_line.obj"},
        Model{"models/stop_line.obj"},
        Model{"models/give_way_line.obj"},
        Model{"models/crosswalk.obj"},
        Model{"models/ground_10.obj"},
        Model{"models/ground_20.obj"},
        Model{"models/ground_30.obj"},
        Model{"models/ground_40.obj"},
        Model{"models/ground_50.obj"},
        Model{"models/ground_60.obj"},
        Model{"models/ground_70.obj"},
        Model{"models/ground_80.obj"},
        Model{"models/ground_90.obj"},
        Model{"models/ground_10_end.obj"},
        Model{"models/ground_20_end.obj"},
        Model{"models/ground_30_end.obj"},
        Model{"models/ground_40_end.obj"},
        Model{"models/ground_50_end.obj"},
        Model{"models/ground_60_end.obj"},
        Model{"models/ground_70_end.obj"},
        Model{"models/ground_80_end.obj"},
        Model{"models/ground_90_end.obj"},
        Model{"models/ground_arrow_left.obj"},
        Model{"models/ground_arrow_right.obj"},
        Model{"models/end.obj"},
        Model{"models/calib.obj"},
    };

    ItemsModule();
    ~ItemsModule();

    void update(std::vector<std::shared_ptr<Scene::Item>>& items,
            Pose* selection);

    void render(GLuint shaderProgramId,
            std::vector<std::shared_ptr<Scene::Item>>& items);
};

#endif
