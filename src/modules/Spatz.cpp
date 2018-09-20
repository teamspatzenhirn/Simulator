#include "Spatz.h"

namespace env {

    const Spatz::SystemParams Spatz::sysParams = {
        .l = env::Spatz::AXESDISTANCE,
        .gamma = env::Spatz::AXESMOMENTRATIO,
        .J = env::Spatz::INERTIA,
        .m = env::Spatz::MASS,
        .M = (env::Spatz::MASS
                * Spatz::DIST_COG_TO_REAR_AXLE
                * Spatz::DIST_COG_TO_REAR_AXLE
                + env::Spatz::INERTIA)
             / env::Spatz::AXESDISTANCE
             / env::Spatz::AXESDISTANCE,
        .lambda = env::Spatz::AXESDISTANCE / 2, // Lage des Schwerpunkts
    };

    Spatz::Spatz() : Spatz(0, glm::vec3(0,0,0)) {

    }

    Spatz::Spatz(double t, glm::vec3 pose) 
        : t(t),
          pose(pose),
          s(0),
          vel(),
          acc(),
          rot(),
          V_Mittel(0),
          Vacc(0),
          laser(0),
          sensor_ir_side({false,{false,false}}),
          steerAngle(0),
          steerSensor(0),
          integratedDistance(0),
          alpha_front(0),
          alpha_rear(0) {

    }

    std::ostream& operator << (std::ostream& stream, const Spatz& spatz) {

        stream << "Spatz position: "
            << "["
            << spatz.pose.x
            << ","
            << spatz.pose.y
            << ","
            << spatz.pose.z
            << "]";
        return stream;
    }

    glm::vec2 Spatz::toGlobal(const glm::vec2 &pt) const {

        return glm::vec2(
                cos(pose.z)*pt.x - sin(pose.z)*pt.y + pose.x,
                sin(pose.z)*pt.x + cos(pose.z)*pt.y + pose.y
        );
    }

    glm::vec2 Spatz::toLocal(const glm::vec2 &pt) const {

        return glm::vec2(
                cos(-pose.z)*(pt.x-pose.x) - sin(-pose.z)*(pt.y-pose.y),
                sin(-pose.z)*(pt.x-pose.x) + cos(-pose.z)*(pt.y-pose.y)
        );
    }

    const std::vector<glm::vec2> Spatz::CAM_REGION = {

        glm::vec2(0,1.0), glm::vec2(3,1.5), glm::vec2(3,-1.5), glm::vec2(0,-1.0)
    };
}
