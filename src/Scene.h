#ifndef INC_2019_SCENE_H
#define INC_2019_SCENE_H

#include <string>
#include <vector>
#include <fstream>
#include <memory>

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

            double velocity = 1.5;
            double steeringAngle = 0.1;

        } vesc;

        /*
         * This contains important paramters of the main
         * color camera of the vehicle.
         */
        struct MainCamera {

            Pose pose{0, 0.260, 0.110};

            MainCamera() {
                pose.setEulerAngles(glm::vec3(-10.0f, 180.0f, 0.0f));
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

    struct Obstacle {

        Id<Pose> pose;
    };
    std::vector<Obstacle> obstacles;

    Scene();
    ~Scene();

    bool save(std::string path);
    bool load(std::string path);
};

#endif
