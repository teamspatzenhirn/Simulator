#ifndef INC_2019_MODELSTORE_H
#define INC_2019_MODELSTORE_H

#include "helpers/Model.h"

#include "Scene.h"

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
    Model scaleArrow;

    Model items[ItemType::LAST_ELEMENT];

    ModelStore (std::string resPath) : 
        car{resPath + "models/spatz11.obj"},
        rect{resPath + "models/ground.obj"},
        ring{resPath + "models/ring.obj"},
        marker{resPath + "models/marker.obj"},
        arrow{resPath + "models/arrow.obj"},
        scaleArrow{resPath + "models/scale_arrow.obj"},
        items{ 
            Model{},
            Model{resPath + "models/obstacle.obj"},
            Model{resPath + "models/floorsigns/start_line.obj"},
            Model{resPath + "models/floorsigns/stop_line.obj"},
            Model{resPath + "models/floorsigns/give_way_line.obj"},
            Model{resPath + "models/floorsigns/crosswalk.obj"},
            Model{resPath + "models/floorsigns/ground_10.obj"},
            Model{resPath + "models/floorsigns/ground_20.obj"},
            Model{resPath + "models/floorsigns/ground_30.obj"},
            Model{resPath + "models/floorsigns/ground_40.obj"},
            Model{resPath + "models/floorsigns/ground_50.obj"},
            Model{resPath + "models/floorsigns/ground_60.obj"},
            Model{resPath + "models/floorsigns/ground_70.obj"},
            Model{resPath + "models/floorsigns/ground_80.obj"},
            Model{resPath + "models/floorsigns/ground_90.obj"},
            Model{resPath + "models/floorsigns/ground_10_end.obj"},
            Model{resPath + "models/floorsigns/ground_20_end.obj"},
            Model{resPath + "models/floorsigns/ground_30_end.obj"},
            Model{resPath + "models/floorsigns/ground_40_end.obj"},
            Model{resPath + "models/floorsigns/ground_50_end.obj"},
            Model{resPath + "models/floorsigns/ground_60_end.obj"},
            Model{resPath + "models/floorsigns/ground_70_end.obj"},
            Model{resPath + "models/floorsigns/ground_80_end.obj"},
            Model{resPath + "models/floorsigns/ground_90_end.obj"},
            Model{resPath + "models/floorsigns/ground_arrow_left.obj"},
            Model{resPath + "models/floorsigns/ground_arrow_right.obj"},
            Model{},
            Model{resPath + "models/calib.obj"},
            Model{resPath + "models/island.obj"},
            Model{resPath + "models/floorsigns/barred_area_small.obj"},
            Model{resPath + "models/floorsigns/barred_area_medium.obj"},
            Model{resPath + "models/floorsigns/barred_area_large.obj"},
            Model{resPath + "models/dynamic_obstacle.obj"},
            Model{resPath + "models/pedestrian.obj"},
            Model{resPath + "models/pedestrian.obj"},
            Model{resPath + "models/pedestrian.obj"},
            Model{resPath + "models/turn_lane.obj"},
            Model{resPath + "models/floorsigns/crosswalk_small.obj"},
            Model{resPath + "models/park_section.obj"},
            Model{resPath + "models/park_slots.obj"},
            Model{resPath + "models/no_parking.obj"},
            Model{resPath + "models/start_box.obj"},
            Model{resPath + "models/signs/sign_forbidden.obj"},
            Model{resPath + "models/signs/sign_downhill.obj"},
            Model{resPath + "models/signs/sign_expressway_start.obj"},
            Model{resPath + "models/signs/sign_expressway_end.obj"},
            Model{resPath + "models/signs/sign_giveway.obj"},
            Model{resPath + "models/signs/sign_no_passing.obj"},
            Model{resPath + "models/signs/sign_no_passing_end.obj"},
            Model{resPath + "models/ground.obj"},
            Model{resPath + "models/giraffe.obj"},

            Model{resPath + "models/signs/sign_parking.obj"},
            Model{resPath + "models/signs/sign_right_of_way.obj"},
            Model{resPath + "models/signs/sign_uphill.obj"},
            Model{resPath + "models/signs/sign_pedestrian_island.obj"},
            Model{resPath + "models/signs/sign_zebra.obj"},
            Model{resPath + "models/signs/sign_stop.obj"},
            Model{resPath + "models/signs/sign_turn_left.obj"},
            Model{resPath + "models/signs/sign_turn_right.obj"},
            Model{resPath + "models/signs/sign_speedlimit_10_start.obj"},
            Model{resPath + "models/signs/sign_speedlimit_20_start.obj"},
            Model{resPath + "models/signs/sign_speedlimit_30_start.obj"},
            Model{resPath + "models/signs/sign_speedlimit_40_start.obj"},
            Model{resPath + "models/signs/sign_speedlimit_50_start.obj"},
            Model{resPath + "models/signs/sign_speedlimit_60_start.obj"},
            Model{resPath + "models/signs/sign_speedlimit_70_start.obj"},
            Model{resPath + "models/signs/sign_speedlimit_80_start.obj"},
            Model{resPath + "models/signs/sign_speedlimit_90_start.obj"},
            Model{resPath + "models/signs/sign_speedlimit_10_end.obj"},
            Model{resPath + "models/signs/sign_speedlimit_20_end.obj"},
            Model{resPath + "models/signs/sign_speedlimit_30_end.obj"},
            Model{resPath + "models/signs/sign_speedlimit_40_end.obj"},
            Model{resPath + "models/signs/sign_speedlimit_50_end.obj"},
            Model{resPath + "models/signs/sign_speedlimit_60_end.obj"},
            Model{resPath + "models/signs/sign_speedlimit_70_end.obj"},
            Model{resPath + "models/signs/sign_speedlimit_80_end.obj"},
            Model{resPath + "models/signs/sign_speedlimit_90_end.obj"},

            Model{resPath + "models/signs/landmark_1.obj"},
            Model{resPath + "models/signs/landmark_2.obj"},
            Model{resPath + "models/signs/landmark_3.obj"},
            Model{resPath + "models/signs/landmark_4.obj"},
            Model{resPath + "models/signs/landmark_5.obj"},

            Model{resPath + "models/start_box_signs.obj"},
         } {
    }
};

#endif
