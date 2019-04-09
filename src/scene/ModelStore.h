#ifndef INC_2019_MODELSTORE_H
#define INC_2019_MODELSTORE_H

#include "helpers/Model.h"

/*
 * Simple object, which holds and loads all used models
 * in the simulator. Should be used to avoid loading the
 * same model multiple times.
 */
struct ModelStore {

    Model car;

    Model rect;

    Model ring;
    Model marker;
    Model arrow;

    Model items[49];

    ModelStore (std::string resPath) : 
        car{resPath + "models/spatz.obj"},
        rect{resPath + "models/ground.obj"},
        ring{resPath + "models/ring.obj"},
        marker{resPath + "models/marker.obj"},
        arrow{resPath + "models/arrow.obj"},
        items{ 
            Model{},
            Model{resPath + "models/obstacle.obj"},
            Model{resPath + "models/start_line.obj"},
            Model{resPath + "models/stop_line.obj"},
            Model{resPath + "models/give_way_line.obj"},
            Model{resPath + "models/crosswalk.obj"},
            Model{resPath + "models/ground_10.obj"},
            Model{resPath + "models/ground_20.obj"},
            Model{resPath + "models/ground_30.obj"},
            Model{resPath + "models/ground_40.obj"},
            Model{resPath + "models/ground_50.obj"},
            Model{resPath + "models/ground_60.obj"},
            Model{resPath + "models/ground_70.obj"},
            Model{resPath + "models/ground_80.obj"},
            Model{resPath + "models/ground_90.obj"},
            Model{resPath + "models/ground_10_end.obj"},
            Model{resPath + "models/ground_20_end.obj"},
            Model{resPath + "models/ground_30_end.obj"},
            Model{resPath + "models/ground_40_end.obj"},
            Model{resPath + "models/ground_50_end.obj"},
            Model{resPath + "models/ground_60_end.obj"},
            Model{resPath + "models/ground_70_end.obj"},
            Model{resPath + "models/ground_80_end.obj"},
            Model{resPath + "models/ground_90_end.obj"},
            Model{resPath + "models/ground_arrow_left.obj"},
            Model{resPath + "models/ground_arrow_right.obj"},
            Model{},
            Model{resPath + "models/calib.obj"},
            Model{resPath + "models/island.obj"},
            Model{resPath + "models/barred_area_small.obj"},
            Model{resPath + "models/barred_area_medium.obj"},
            Model{resPath + "models/barred_area_large.obj"},
            Model{resPath + "models/dynamic_obstacle.obj"},
            Model{resPath + "models/pedestrian.obj"},
            Model{resPath + "models/pedestrian.obj"},
            Model{resPath + "models/pedestrian.obj"},
            Model{resPath + "models/turn_lane.obj"},
            Model{resPath + "models/crosswalk_small.obj"},
            Model{resPath + "models/park_section.obj"},
            Model{resPath + "models/park_slots.obj"},
            Model{resPath + "models/no_parking.obj"},
            Model{resPath + "models/start_box.obj"},
            Model{resPath + "models/sign_forbidden.obj"},
            Model{resPath + "models/sign_downhill.obj"},
            Model{resPath + "models/sign_expressway_start.obj"},
            Model{resPath + "models/sign_expressway_end.obj"},
            Model{resPath + "models/sign_giveway.obj"},
            Model{resPath + "models/sign_no_passing.obj"},
            Model{resPath + "models/sign_no_passing_end.obj"}
         } {
    }
};

#endif
