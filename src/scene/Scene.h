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
    OBSTACLE,
    START_LINE,
    STOP_LINE,
    GIVE_WAY_LINE,
    CROSSWALK,
    GROUND_10,
    GROUND_20,
    GROUND_30,
    GROUND_40,
    GROUND_50,
    GROUND_60,
    GROUND_70,
    GROUND_80,
    GROUND_90,
    GROUND_10_END,
    GROUND_20_END,
    GROUND_30_END,
    GROUND_40_END,
    GROUND_50_END,
    GROUND_60_END,
    GROUND_70_END,
    GROUND_80_END,
    GROUND_90_END,
    GROUND_ARROW_LEFT,
    GROUND_ARROW_RIGHT,
    CHECKPOINT,
    CALIB_MAT,
    TRAFFIC_ISLAND,
    BARRED_AREA_SMALL,
    BARRED_AREA_MEDIUM,
    BARRED_AREA_LARGE,
    DYNAMIC_OBSTACLE,
    PEDESTRIAN,
    DYNAMIC_PEDESTRIAN_RIGHT,
    DYNAMIC_PEDESTRIAN_LEFT,
    TURN_LANE,
    CROSSWALK_SMALL,
    PARK_SECTION,
    PARK_SLOTS,
    NO_PARKING,
    START_BOX,
    SIGN_FORBIDDEN,
    SIGN_DOWNHILL,
    SIGN_EXPRESSWAY_START,
    SIGN_EXPRESSWAY_END,
    SIGN_GIVEWAY,
    SIGN_NO_PASSING_START,
    SIGN_NO_PASSING_END,
    MISSING_SPOT,
    GIRAFFE,
    SIGN_PARKING_AREA,
    SIGN_RIGHT_OF_WAY,
    SIGN_UPHILL,
    SIGN_PEDESTRIAN_ISLAND,
    SIGN_CROSSWALK,
    SIGN_STOP,
    SIGN_TURN_LEFT,
    SIGN_TURN_RIGHT,
    SIGN_10,
    SIGN_20,
    SIGN_30,
    SIGN_40,
    SIGN_50,
    SIGN_60,
    SIGN_70,
    SIGN_80,
    SIGN_90,
    SIGN_10_END,
    SIGN_20_END,
    SIGN_30_END,
    SIGN_40_END,
    SIGN_50_END,
    SIGN_60_END,
    SIGN_70_END,
    SIGN_80_END,
    SIGN_90_END,
    START_BOX_SIGNS,
    LAST_ELEMENT
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
     * A changelog describing the changes can be found at the end of this file.
     */
    static const unsigned int VERSION = 5;

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

/*
 * Version changelog:
 * - V1: unknown
 * - V2: unknown
 * - V3: unknown
 * - V4: unknown
 * - V5: added new signs (SIGN_PARKING_AREA, SIGN_RIGHT_OF_WAY, SIGN_UPHILL, SIGN_PEDESTRIAN_ISLAND,
         SIGN_CROSSWALK, SIGN_STOP, SIGN_TURN_LEFT, SIGN_TURN_RIGHT, SIGN_10 to SIGN_90,
         SIGN_10_END to SIGN_90_END)
 */
