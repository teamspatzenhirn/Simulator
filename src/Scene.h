#ifndef INC_2019_SCENE_H
#define INC_2019_SCENE_H

#include <deque>
#include <string>
#include <vector>
#include <fstream>
#include <memory>
#include <cstdlib>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

#include <helpers/Helpers.h>

struct TrackBase;

struct ControlPoint {

    glm::vec2 coords;

    std::vector<std::shared_ptr<TrackBase>> tracks;
};

struct TrackBase {

    std::weak_ptr<ControlPoint> start;
    std::weak_ptr<ControlPoint> end;

    TrackBase(const std::shared_ptr<ControlPoint>& start, const std::shared_ptr<ControlPoint>& end);
    virtual ~TrackBase() = 0;

    virtual glm::vec2 getDirection(const ControlPoint& controlPoint) = 0;
};

struct TrackLine : TrackBase {

    TrackLine(const std::shared_ptr<ControlPoint>& start, const std::shared_ptr<ControlPoint>& end);

    glm::vec2 getDirection(const ControlPoint& controlPoint) override;
};

struct TrackArc : TrackBase {

    glm::vec2 center;
    float radius{0};
    bool rightArc{false};

    TrackArc(const std::shared_ptr<ControlPoint>& start, const std::shared_ptr<ControlPoint>& end,
            const glm::vec2& center, const float radius, const bool rightArc);

    glm::vec2 getDirection(const ControlPoint& controlPoint) override;
};

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
    END = 26,
    CALIB_MAT = 27,
};

/* 
 * This contains settings that are not stored per config
 * but globally as a ~/.config-file.
 */
struct Settings {

    Settings () {

        std::string strHomePath("/");
        char* homePath = getenv("HOME");

        if (homePath) {
            strHomePath = std::string(homePath);
        } else {
            homePath = getenv("HOMEPATH");
            if (homePath) {
                strHomePath = std::string(homePath);
            }
        }

        settingsFilePath = strHomePath + "/.carolosim";
    }

    /*
     * The path where the global settings file is stored.
     */
    std::string settingsFilePath;

    /*
     * The path to the last opened config file or the path set by
     * command line argument.
     */
    std::string configPath;

    /*
     * The speed of the simulation given as fraction of real time.
     */
    float simulationSpeed = 0.25f;

    /*
     * If set marker/modifier points will be rendered.
     */
    bool showMarkers = true;

    /*
     * If set the path of the vehicle, that is the position
     * history will be draw.
     */
    bool showVehiclePath = true;

    /*
     * If set the vehicle path will be draw in the prettiest
     * rainbow colors one can imagine.
     */
    bool fancyVehiclePath = true;

    /*
     * If set the vehicle trajectory points set in 
     * "visualization" struct will be drawn.
     */
    bool showVehicleTrajectory = true;

    /*
     * This saves the settings to ~/.carolosim
     */
    bool save();

    /*
     * This tries to loads the settings from ~/.carolosim
     */
    bool load();
};

/*
 * In order to make simulation state propagration and retention
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
     * The current time of the simulation in milliseconds.
     */
    double simulationTime = 0;

    /*
     * This is free camera that is used in the editor.
     */
    FpsCamera fpsCamera{{0, 1, 1.5}, 0.5, 0, M_PI * 0.3f, 4.0f/3.0f};

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
     * This struct contains the state of the simulated model car.
     */
    struct Car {

        Pose modelPose{0.0, 0.0, 0.0};

        glm::vec3 velocity{0, 0, 0};
        glm::vec3 acceleration{0, 0, 0};

        double steeringAngle = 0;

        double alphaFront = 0;
        double alphaRear = 0;

        struct SystemParams {

            // radabstand (m)
            double axesDistance = 0.225; 

            // Verhältnis des Moments vorder/hinter Achse;
            double axesMomentRatio = 0.5; 
            
            // kg*m^2 
            double inertia = 0.042; 

            // masse (kg)
            double mass = 3.875; 

            // center of gravity to front axle
            double distCogToFrontAxle = axesDistance / 2.0;

            // center of gravity to rear axle
            double distCogToRearAxle = axesDistance - distCogToFrontAxle;
           
            double getM () {

                return (mass
                    * distCogToRearAxle
                    * distCogToRearAxle
                    + inertia)
                 / axesDistance
                 / axesDistance;
            }

        } systemParams;

        struct SimulatorState {

            double x1 = 0;
            double x2 = 0;
            double psi = 0;
            double delta = 0;
            double v = 0;
            double v_lon = 0;
            double v_lat = 0; 
            double d_psi = 0;     

        } simulatorState;

        struct Limits {

            double max_F = 5;
            double max_delta = 0.35;
            double max_d_delta = 4;

        } limits;

        struct Wheels {

            bool usePacejkaModel = true;

            double B_front = 1;
            double B_rear = 1;
            double C_front = 1.9;
            double C_rear = 1.9;
            double D_front = 12;
            double D_rear = 12;
            double k_front = 10;
            double k_rear = 10;

        } wheels;

        /*
         * These are the parameters that are send to the VESC motor
         * controller.
         */
        struct Vesc {

            double velocity = 0;
            double steeringAngle = 0;

        } vesc;

        /*
         * This contains important paramters of the main
         * color camera of the vehicle.
         */
        struct MainCamera {

            Pose pose{0, 0.260, 0.110};

            MainCamera() {
                pose.setEulerAngles(glm::vec3(-12.0f, 180.0f, 0.0f));
            }

            unsigned int imageWidth = 2064;
            unsigned int imageHeight = 1544;

            float fovy = M_PI * 0.5f;

            struct DistortionCoefficients {

                float radial[3] = {0, 0, 0};
                float tangential[3] = {0, 0, 0};

            } distortionCoefficients;

            float getAspectRatio() {
                return (float)imageWidth / (float)imageHeight;
            }

        } mainCamera;

        /*
         * This contains the parameters for the color and the depth images
         * produced by the depth camera.
         */
        struct DepthCamera {

            Pose pose{0, 0.19, 0.05};

            DepthCamera() {
                pose.setEulerAngles(glm::vec3(0.0f, 180.0f, 0.0f));
            }

            unsigned int colorImageWidth = 1280;
            unsigned int colorImageHeight = 720;

            unsigned int depthImageWidth = 640;
            unsigned int depthImageHeight = 480;

            float colorFovy = M_PI * 0.5f;
            float depthFovy = M_PI * 0.25f;

            float getColorAspectRatio() {
                return (float)colorImageWidth / (float)colorImageHeight;
            }

            float getDepthAspectRatio() {
                return (float)depthImageWidth / (float)depthImageHeight;
            }

        } depthCamera;

    } car;

    float groundSize = 10.0f;

    struct Tracks {

        // total width of a track
        float trackWidth = 0.8f;

        // lane markings
        float markingWidth = 0.02f;
        float centerLineLength = 0.2f;
        float centerLineInterrupt = 0.2f;

    private:

        std::vector<std::shared_ptr<ControlPoint>> tracks;

    public:

        const std::vector<std::shared_ptr<ControlPoint>>& getTracks() const;

        std::shared_ptr<TrackLine> addTrackLine(const std::shared_ptr<ControlPoint>& start, const std::shared_ptr<ControlPoint>& end);
        std::shared_ptr<TrackArc> addTrackArc(const std::shared_ptr<ControlPoint>& start, const std::shared_ptr<ControlPoint>& end, const glm::vec2& center, const float radius, const bool rightArc);

        bool controlPointExists(const std::shared_ptr<ControlPoint>& controlPoint) const;

        void removeControlPoint(std::shared_ptr<ControlPoint>& controlPoint);

    } tracks;

    /*
     * An item is everything that is not the car or the track.
     * E.g. street signs, road markings, obstacles ...
     */
    struct Item {

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

    std::vector<std::shared_ptr<Item>> items;

    /*
     * This contains the RuleModule state and the set
     * reactions to violations of the Carolo Cup rules.
     */
    struct Rules {

        std::shared_ptr<Item> line;
        uint64_t lineTime = 0;
        bool linePassed = false;

        std::shared_ptr<Item> rightArrow;
        std::shared_ptr<Item> leftArrow;

        float allowedMaxSpeed = 1000;

        bool onTrack = false;
        bool isColliding = false;

        bool exitOnObstacleCollision = false;
        bool exitIfNotOnTrack = false;
        bool exitIfSpeedLimitExceeded = false;
        bool exitIfLeftArrowIgnored = false;
        bool exitIfRightArrowIgnored = false;
        bool exitIfStopLineIgnored = false;
        bool exitIfGiveWayLineIgnored = false;
        bool exitIfOnEndItem = false;

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
     * Adds a this scene to the scene history.
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
    static Scene& getHistoryBackStep(int step);
};

#endif
