#include "Spatz.h"

namespace env {

    const Spatz::SystemParams Spatz::sysParams = {
        .l = env::Spatz::AXESDISTANCE,
        .gamma = env::Spatz::AXESMOMENTRATIO,
        .J = env::Spatz::INERTIA,
        .m = env::Spatz::MASS,
        .M = (env::Spatz::MASS * Spatz::DIST_COG_TO_REAR_AXLE * Spatz::DIST_COG_TO_REAR_AXLE + env::Spatz::INERTIA)
             / env::Spatz::AXESDISTANCE / env::Spatz::AXESDISTANCE,
        .lambda = env::Spatz::AXESDISTANCE/2, // Lage des Schwerpunkts
    };

    const double Spatz::WIDTH = 0.190;
    const double Spatz::LENGTH = 0.295;
    const double Spatz::ORIGIN_X = 0.032;
    const double Spatz::ORIGIN_Y = 0;
    const double Spatz::AXESDISTANCE = 0.225;           // radabstand (m)
    const double Spatz::MASS = 3.875;                     // masse (kg)
    const double Spatz::WHEELRADIUS = 0.020;            // rad radius (m)

    const double Spatz::SCHRAEGLAUFUEBERSETZUNG = 2;
    const double Spatz::INERTIA = 0.042; // kg*m^2; grobe Schätzung über Quader und Masse
    const double Spatz::FRICTIONCOEFF = 3;
    const double Spatz::AIREFFCW = 0.2;                 // m^2
    const double Spatz::COM_X = Spatz::AXESDISTANCE/2;
    const double Spatz::COM_Y = 0;
    const double Spatz::GEARRATIO = 4000;
    const double Spatz::STEERT = 0.01;                  // Lenkwinkelverzögerungskonstante, eingangsaffines System mit PT1 (s)
    const double Spatz::MAX_STEERING_ANGLE = 22.5*M_PI/180; // maximal 18.5 Grad, um Reibung an Feder zu vermeiden
    const double Spatz::AXESMOMENTRATIO = 0.5;          // Verhältnis des Moments vorder/hinter Achse

    const double Spatz::ACC_TO_CURRENT_RATIO = 0.4338; // Verhältnis Beschleunigung zum vorgegebenen Strom
    const double Spatz::STATIC_FRICTION = 0.2536; // constant negative acceleration (m/s^2)
    const double Spatz::STATIC_STEER_FRICTION = 5.086; // constant negative acceleration per squared steering angle (m/s^2 per rad^2)
    const double Spatz::DYNAMIC_FRICTION = 0.3990; // negative acceleration per speed (m/s^2 per m/s)
    const double Spatz::DYNAMIC_STEER_FRICTION = 0.5964; // negative acceleration per speed and steering angle (m/s^2 per rad*m/s)

    const double Spatz::DIST_COG_TO_FRONT_AXLE = AXESDISTANCE/2; // center of gravity to front axle
    const double Spatz::DIST_COG_TO_REAR_AXLE = AXESDISTANCE - DIST_COG_TO_FRONT_AXLE; // center of gravity to rear axle

    Spatz::Spatz() : Spatz(0, cv::Point3d(0,0,0)) { }
    Spatz::Spatz(double t, cv::Point3d pose) : t(t), pose(pose), s(0), vel(), acc(), rot(), V_Mittel(0),Vacc(0), laser(0), sensor_ir_side({false,{false,false}}),
        steerAngle(0), steerSensor(0), integratedDistance(0), alpha_front(0), alpha_rear(0) { }

    std::ostream& operator << (std::ostream& stream, const Spatz& spatz) {
        stream << "spatz @ " << spatz.pose;
        return stream;
    }

    cv::Point2d Spatz::toGlobal(const cv::Point2d &pt) const {
        return cv::Point2d(
                cos(pose.z)*pt.x - sin(pose.z)*pt.y + pose.x,
                sin(pose.z)*pt.x + cos(pose.z)*pt.y + pose.y
        );
    }

    cv::Point2d Spatz::toLocal(const cv::Point2d &pt) const {
        return cv::Point_<double>(
                cos(-pose.z)*(pt.x-pose.x) - sin(-pose.z)*(pt.y-pose.y),
                sin(-pose.z)*(pt.x-pose.x) + cos(-pose.z)*(pt.y-pose.y)
        );
    }

    const std::vector<cv::Point2f> Spatz::CAM_REGION = {
            cv::Point2d(0,1.0), cv::Point2d(3,1.5), cv::Point2d(3,-1.5), cv::Point2d(0,-1.0)
    };

}
