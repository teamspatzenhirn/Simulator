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
    MISSING_SPOT = 49,
    GIRAFFE = 50,
    SIGN_PARKING_AREA = 51,
    SIGN_RIGHT_OF_WAY = 52,
    SIGN_UPHILL = 53,
    SIGN_PEDESTRIAN_ISLAND = 54,
    SIGN_CROSSWALK = 55,
    SIGN_STOP = 56,
    SIGN_TURN_LEFT = 57,
    SIGN_TURN_RIGHT = 58,
    SIGN_30 = 59,
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
    static const unsigned int VERSION = 4;

    /*
     * This is the actual version of the scene object.
     */
    unsigned int version = VERSION;
    
    /*
     * If set the simulation clock will NOT be updated.
     */
    bool paused = false;

    /*
     * Variable set at the first rule violation.
     * Is used to restart the simulator if autotracks is enabled.
     */
    double failTime = 0.0;

    /*
     * This clock controls the updates of the non-simulation logic.
     */
    Clock displayClock;

    /*
     * This clock controls the simulation updates.
     */
    Clock simulationClock;

    /*
     * This is the free camera that is used in the editor.
     */
    FpsCamera fpsCamera{{0, 1, 1.5}, 0.5, 0, (float)M_PI * 0.3f, 4.0f/3.0f};

    /*
     * This is a top view camera that is following the car.
     */
    FollowCamera followCamera;

    CinematicCamera cinematicCamera;

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
     * The point light source.
     */
    PointLight light{5.0f, 5.0f, 5.0f};

    /*
     * The size of the ground plate on which tracks can be placed.
     */
    float groundSize = 40.0f;

    /*
     * Manager objects for the tracks (the track graph).
     */
    Tracks tracks;

    /*
     * If set, procedural track generation is enabled.
     */
    bool enableAutoTracks = false;

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
        
        explicit Item (ItemType type) : type(type) {
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

        double lastIteractionTime = -1;
        double lastDrivenDistance = 0;

        bool isColliding = false;
        bool onTrack = false;
        bool speedLimitExceeded = false;
        bool leftArrowIgnored = false;
        bool rightArrowIgnored = false;
        bool stopLineIgnored = false;
        bool giveWayLineIgnored = false;
        bool crosswalkIgnored = false;
        bool noParkingIgnored = false;
        bool lackOfProgress = false;

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
