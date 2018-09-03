#ifndef ENV_SPATZ_H
#define ENV_SPATZ_H

#include <cstdint>
#include <vector>
#include <ostream>

#include <glm/glm.hpp>

namespace env {

    class Spatz {

    public:

        struct SystemParams {
            double l,
            gamma,
            J, m, M,
            lambda; // Lage des Schwerpunkts
        };

        static const SystemParams sysParams;

        static const double WIDTH;
        static const double LENGTH;
        static const double ORIGIN_X;
        static const double ORIGIN_Y;
        static const double AXESDISTANCE; // radabstand (m)
        static const double MASS; // masse (kg)
        static const double WHEELRADIUS; // rad radius (m)

        static const double SCHRAEGLAUFUEBERSETZUNG;
        static const double INERTIA; // kg*m^2
        static const double FRICTIONCOEFF;
        static const double AIREFFCW; // m^2
        static const double COM_X;
        static const double COM_Y;
        static const double GEARRATIO;
        static const double STEERT; // Lenkwinkelverzögerungskonstante, eingangsaffines System mit PT1 (s)
        static const double MAX_STEERING_ANGLE; // maximal 18.5 Grad, um Reibung an Feder zu vermeiden
        static const double AXESMOMENTRATIO; // Verhältnis des Moments vorder/hinter Achse

        static const double ACC_TO_CURRENT_RATIO; // Verhältnis Beschleunigung zum vorgegebenen Strom
        static const double STATIC_FRICTION; // constant negative acceleration (m/s^2)
        static const double STATIC_STEER_FRICTION; // constant negative acceleration per squared steering angle (m/s^2 per rad^2)
        static const double DYNAMIC_FRICTION; // negative acceleration per speed (m/s^2 per m/s)
        static const double DYNAMIC_STEER_FRICTION; // negative acceleration per speed and steering angle (m/s^2 per rad*m/s)

        static const double DIST_COG_TO_FRONT_AXLE; // center of gravity to front axle
        static const double DIST_COG_TO_REAR_AXLE; // center of gravity to rear axle
        static const std::vector<glm::vec2> CAM_REGION;

    private:

        double t; // Offset 0*8
        glm::vec3 pose; // x,y,psi, Offset 1*8
        double s; // DEPRECATED, Offset 4*8
        glm::vec3 vel; // Offset 5*8
        glm::vec3 acc; // Offset 8*8
        glm::vec3 rot; // Offset 11*8
        glm::vec3 drot; // Offset 14*8

        double theta_unused; // Lenkwinkel, mathematisch im FahrzeugCS, Offset 17*8
        double M; // Moment, Offset 18*8

        double V_Mittel; // Offset 19*8
        double Vacc; // Ableitung der gemittelten Geschwindigkeit, Offset 20*8

        // sensors
        double laser; // Offset 21*8
        double steerAngle; // Offset 22*8
        int16_t steerSensor; // Lenkwinkelsensor, Offset 23*8
        std::pair<bool, std::pair<bool,bool>> sensor_ir_side; // < newvalue?, <vorne, hinten> >

        double integratedDistance;

    public:

        Spatz();
        Spatz(double t, glm::vec3 pose);

        glm::vec2 toGlobal(const glm::vec2& pt) const;
        glm::vec2 toLocal(const glm::vec2& pt) const;

        double alpha_front, alpha_rear; // Schräglaufwinkel

        const glm::vec2 getPos() const {
            return glm::vec2(pose.x, pose.y);
        }

        const double getPsi() const {
            return pose.z;
        }

        const glm::vec3 &getPose() const {
            return pose;
        }

        void setPose(const glm::vec3 &pose) {
            Spatz::pose = pose;
        }

        double getS() const {
            return s;
        }

        void setS(double s) {
            Spatz::s = s;
        }

        const glm::vec3 &getVel() const {
            return vel;
        }

        void setVel(const glm::vec3 &vel) {
            Spatz::vel = vel;
        }

        const glm::vec3 &getAcc() const {
            return acc;
        }

        void setAcc(const glm::vec3 &acc) {
            Spatz::acc = acc;
        }

        double getM() const {
            return M;
        }

        void setM(double M) {
            Spatz::M = M;
        }

        const double getTseconds() const {
            return t;
        }

        const int64_t getTmicros() const {
            return (int64_t)(t * 1000000);
        }

        void setT(double t) {
            Spatz::t = t;
        }

        void setT(int64_t t) {
            Spatz::t = t / 1000000.0;
        }

        const glm::vec3 &getRot() const {
            return rot;
        }

        void setRot(const glm::vec3 &rot) {
            Spatz::rot = rot;
        }

        const glm::vec3 &getdRot() const {
            return drot;
        }

        void setdRot(const glm::vec3 &drot) {
            Spatz::drot = drot;
        }

        double getVacc() const {
            return Vacc;
        }

        void setVacc(double Vacc) {
            Spatz::Vacc = Vacc;
        }

        double getV_Mittel() const {
            return V_Mittel;
        }

        void setV_Mittel(double V_Mittel) {
            Spatz::V_Mittel = V_Mittel;
        }

        const std::pair<bool, std::pair<bool,bool>> &getSensorIrSide() const {
            return sensor_ir_side;
        }

        void setSensorIrSide(const std::pair<bool, std::pair<bool,bool>> &sensor_ir_side) {
            Spatz::sensor_ir_side = sensor_ir_side;
        }

        const double& getLaser() const {
            return laser;
        }

        void setLaser(const double& laser) {
            Spatz::laser = laser;
        }

        const int16_t& getSteerSensor() const {
            return steerSensor;
        }

        void setSteerSensor(const int16_t& steerSensor) {
            Spatz::steerSensor = steerSensor;
        }

        const double& getSteerAngle() const {
            return steerAngle;
        }

        void setSteerAngle(const double& steerAngle) {
            Spatz::steerAngle = steerAngle;
        }

        double getIntegratedDistance() const {
            return integratedDistance;
        }

        void setIntegratedDistance(double integratedDistance) {
            Spatz::integratedDistance = integratedDistance;
        }

        friend std::ostream& operator<< (std::ostream& stream, const Spatz& spatz);
    };
}

#endif //ENV_SPATZ_H
