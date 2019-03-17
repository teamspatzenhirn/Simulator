#ifndef INC_2019_SCENE_H
#define INC_2019_SCENE_H

#include <deque>
#include <string>
#include <vector>
#include <set>
#include <fstream>
#include <memory>
#include <cstdlib>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

#include <helpers/Helpers.h>

#include "scene/Tracks.h"
#include "scene/Settings.h"
#include "scene/Car.h"

/*
 * For easier access (less typing, that is) the enum
 * is placed here and not in Scene.
 */
enum ItemType {

    NONE = 0,
    OBSTACLE = 1,
    START_LINE = 2,
    STOP_LINE = 3,
    GIVE_WAY_LINE = 4,
    CROSSWALK = 5,
    GROUND_10 = 6,
    GROUND_20 = 7,
    GROUND_30 = 8,
    GROUND_40 = 9,
    GROUND_50 = 10,
    GROUND_60 = 11,
    GROUND_70 = 12,
    GROUND_80 = 13,
    GROUND_90 = 14,
    GROUND_10_END = 15,
    GROUND_20_END = 16,
    GROUND_30_END = 17,
    GROUND_40_END = 18,
    GROUND_50_END = 19,
    GROUND_60_END = 20,
    GROUND_70_END = 21,
    GROUND_80_END = 22,
    GROUND_90_END = 23,
    GROUND_ARROW_LEFT = 24,
    GROUND_ARROW_RIGHT = 25,
    CHECKPOINT = 26,
    CALIB_MAT = 27,
    TRAFFIC_ISLAND = 28,
    BARRED_AREA_SMALL = 29,
    BARRED_AREA_MEDIUM = 30,
    BARRED_AREA_LARGE = 31,
    DYNAMIC_OBSTACLE = 32,
    PEDESTRIAN = 33,
    DYNAMIC_PEDESTRIAN_RIGHT = 34,
    DYNAMIC_PEDESTRIAN_LEFT = 35,
    TURN_LANE = 36,
    CROSSWALK_SMALL = 37,
    PARK_SECTION = 38,
    PARK_SLOTS = 39,
    NO_PARKING = 40,
    START_BOX = 41,
    SIGN_FORBIDDEN = 42,
    SIGN_DOWNHILL = 43,
    SIGN_EXPRESSWAY_START = 44,
    SIGN_EXPRESSWAY_END = 45,
    SIGN_GIVEWAY = 46,
    SIGN_NO_PASSING_START = 47,
    SIGN_NO_PASSING_END = 48,
};

/*
 * Simple object, which holds and loads all used models
 * in the simulator. Should be used to avoid loading the
 * same model multiple times.
 */
struct ModelStore {

    Model itemModels[49] = {
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
        Model{},
        Model{"models/calib.obj"},
        Model{"models/island.obj"},
        Model{"models/barred_area_small.obj"},
        Model{"models/barred_area_medium.obj"},
        Model{"models/barred_area_large.obj"},
        Model{"models/dynamic_obstacle.obj"},
        Model{"models/pedestrian.obj"},
        Model{"models/pedestrian.obj"},
        Model{"models/pedestrian.obj"},
        Model{"models/turn_lane.obj"},
        Model{"models/crosswalk_small.obj"},
        Model{"models/park_section.obj"},
        Model{"models/park_slots.obj"},
        Model{"models/no_parking.obj"},
        Model{"models/start_box.obj"},
        Model{"models/sign_forbidden.obj"},
        Model{"models/sign_downhill.obj"},
        Model{"models/sign_expressway_start.obj"},
        Model{"models/sign_expressway_end.obj"},
        Model{"models/sign_giveway.obj"},
        Model{"models/sign_no_passing.obj"},
        Model{"models/sign_no_passing_end.obj"},
    };
};

/*
 * In order to make simulation state propagation and retention
 * as simple as possible the complete simulation state should be
 * encapsulated in this Scene object.
 *
 * That means, that a Scene could contain, for example, the state of the car,
 * the state of all track elements, the position, scale, rotation of obstacles
 * or simply all other relevant information that should be retained between
 * simulator runs or is used by different modules.
 */
struct Scene {

    /*
     * This is the current most recent version of the Scene object.
     * Increment this, whenever there were changes made to Scene.
     */
    static const unsigned int VERSION = 3;

    /*
     * This is the actual version of the scene object.
     */
    unsigned int version = VERSION;
    
    /*
     * If set the simulation will be paused.
     */
    bool paused = false;

    /*
     * This timer controls the updates of the non-simulation logic.
     */
    Timer displayTimer;

    /*
     * This timer controls the simulation updates.
     */
    Timer simulationTimer;

    /*
     * The current time of the simulation in seconds.
     */
    double simulationTime = 0;

    /*
     * This is the free camera that is used in the editor.
     */
    FpsCamera fpsCamera{{0, 1, 1.5}, 0.5, 0, (float)M_PI * 0.3f, 4.0f/3.0f};

    struct Selection {

        /*
         * This is the currently selected item / object in the scene.
         */
        Pose* pose = nullptr;

        /*
         * This checks if a left mouse click was handled.
         */
        bool handled = false;

    } selection;

    /*
     * The state of the simulated model car.
     */
    Car car;

    /*
     * The size of the ground plate on which tracks can be placed.
     */
    float groundSize = 10.0f;

    /*
     * Manager objects for the tracks (the track graph).
     */
    Tracks tracks;

    /*
     * An item is everything that is not the car or the track.
     * E.g. street signs, road markings, obstacles ...
     */
    struct Item {

        uint64_t id = getId();
        ItemType type = NONE;
        Pose pose{0, 0, 0};
        std::string name = "unnamed_item";

        Item () {
        }
        
        Item (ItemType type) : type(type) {
        }

        Item (ItemType type, std::string name) : type(type), name(name) {
        }
    };

    std::vector<Item> items;

    /*
     * These are additional settings for the dynamic items.
     *
     * TODO: remove this when switching to a proper component system
     */
    struct DynamicItemSettings {

        float speed = 0.6f;

    } dynamicItemSettings;

    /*
     * This contains the RuleModule state and the set
     * reactions to violations of the Carolo Cup rules.
     */
    struct Rules {

        uint64_t lineId = 0;
        double lineTime = 0;
        bool linePassed = false;

        uint64_t rightArrowId = 0;
        uint64_t leftArrowId = 0;

        std::vector<uint64_t> passedCheckpointIds;

        float allowedMaxSpeed = 1000;

        bool isColliding = false;
        bool onTrack = false;
        bool speedLimitExceeded = false;
        bool leftArrowIgnored = false;
        bool rightArrowIgnored = false;
        bool stopLineIgnored = false;
        bool giveWayLineIgnored = false;
        bool crosswalkIgnored = false;
        bool noParkingIgnored = false;

        bool exitOnObstacleCollision = false;
        bool exitIfNotOnTrack = false;
        bool exitIfSpeedLimitExceeded = false;
        bool exitIfLeftArrowIgnored = false;
        bool exitIfRightArrowIgnored = false;
        bool exitIfStopLineIgnored = false;
        bool exitIfGiveWayLineIgnored = false;
        bool exitIfCrosswalkIgnored = false;
        bool exitIfNoParkingIgnored = false;
        bool exitIfAllCheckpointsPassed = false;

    } rules;
    
    /*
     * This struct contains visualization information
     * like the trajectory, ...
     */
    struct Visualization {

        std::vector<glm::vec2> trajectoryPoints;

    } visualization;

    Scene();
    Scene(std::string path);

    ~Scene();

    bool save(std::string path);
    bool load(std::string path);

    /*
     * Contains the last 10 simulated seconds of Scene objects.
     */
    static std::deque<Scene> history;

    /*
     * Adds this scene to the scene history.
     */
    void addToHistory();

    /*
     * Returns the first scene object from the history which
     * time stamp is <= the given time point.
     */
    static Scene& getFromHistory(float simulationTimePoint);

    /*
     * Returns the nth-last element from the history.
     */
    static Scene& getHistoryBackStep(size_t step);
};

#endif
