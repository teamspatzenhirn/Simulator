#include "SpatzSimulator.h"

SpatzSimulator::SpatzSimulator() {

    this->sys = {0};
    this->limits = {0};
    reset();
}

void SpatzSimulator::reset() {

    x.x1 = 0;
    x.x2 = 0;
    x.psi = 0;
    x.delta = 0;
    x.v = 0;
    x.v_lon = 0;
    x.v_lat = 0;
    x.d_psi = 0;
}

void SpatzSimulator::setSystemParams(env::Spatz::SystemParams system) {

    this->sys = system;
}

void SpatzSimulator::setWheelParams(WheelParams wheelParams) {

    this->wheelParams = wheelParams;
}

void SpatzSimulator::setLimits(Limits limits) {

    this->limits = limits;
}

env::Spatz SpatzSimulator::step(ControlVarsVESC ctrlVars, double dt, bool pacejkaModel) {

    double acc = (ctrlVars.vel - x.v_lon) / dt;
    acc = std::min(acc, limits.max_F / env::Spatz::MASS);
    acc = std::max(acc, -limits.max_F / env::Spatz::MASS);

    double F = acc * env::Spatz::MASS;
    F = std::min(F, limits.max_F);
    F = std::max(F, -limits.max_F);

    X dx;
    dx.delta = (ctrlVars.delta - x.delta) / dt;
    dx.delta = std::min(dx.delta, limits.max_d_delta);
    dx.delta = std::max(dx.delta, -limits.max_d_delta);
    double alpha_front = 0, alpha_rear = 0;

    if (!pacejkaModel || x.v_lon <= 0) {
        dx.x1 = x.v * std::cos(x.psi);
        dx.x2 = x.v * std::sin(x.psi);
        dx.psi = x.v * std::tan(x.delta) / sys.l;
        double cos_ = std::cos(x.delta);
        double tan_ = std::tan(x.delta);

        dx.v = ((1 + sys.gamma * (1 / cos_ - 1)) * F
                - 2 * sys.M * x.v * tan_ / (cos_ * cos_) * dx.delta)
               / (sys.m + sys.M * (tan_ * tan_));

        x.x1 += dt * dx.x1;
        x.x2 += dt * dx.x2;
        x.psi += dt * dx.psi;
        x.v += dt * dx.v;
        x.d_psi = dx.psi;

        x.v_lon = x.v;
        dx.v_lon = dx.v;
        dx.v_lat = 0;
    } else {

        dx.x1 = x.v_lon*std::cos(x.psi) + x.v_lat*std::sin(x.psi);
        dx.x2 = x.v_lon*std::sin(x.psi) - x.v_lat*std::cos(x.psi);
        dx.psi = x.d_psi;

        alpha_front = -std::atan2(dx.psi*env::Spatz::DIST_COG_TO_FRONT_AXLE + x.v_lat, std::abs(x.v_lon)) + x.delta; //abs um korrekten Schräglaufwinkel fürs Rückwärts fahren zu erhalten
        alpha_rear = std::atan2(dx.psi*env::Spatz::DIST_COG_TO_REAR_AXLE - x.v_lat, std::abs(x.v_lon));

        if (std::abs(alpha_front) > 0.45 || std::abs(alpha_rear) > 0.45) {
            std::cout << "Schräglaufwinkel zu hoch!" << alpha_front;
        }

        const double F_front_lat = sys.m*wheelParams.D_front*std::sin(wheelParams.C_front*std::atan(wheelParams.B_front*alpha_front));
        const double F_rear_lat = sys.m*wheelParams.D_rear*std::sin(wheelParams.C_rear*std::atan(wheelParams.B_rear*alpha_rear));
        const double F_front_lon = F * sys.gamma;
        const double F_rear_lon = F * (1-sys.gamma);

        dx.v_lon = 1/sys.m * (F_rear_lon - F_front_lat*std::sin(x.delta) + F_front_lon*std::cos(x.delta) + sys.m*x.v_lat*x.d_psi);
        dx.v_lat = 1/sys.m * (F_rear_lat + F_front_lat*std::cos(x.delta) + F_front_lon*std::sin(x.delta) - sys.m*x.v_lon*x.d_psi);
        dx.d_psi = 1/sys.J * ((F_front_lat*std::cos(x.delta) + F_front_lon*std::sin(x.delta))*env::Spatz::DIST_COG_TO_FRONT_AXLE - F_rear_lat*env::Spatz::DIST_COG_TO_REAR_AXLE);

        x.x1 += dt * dx.x1;
        x.x2 += dt * dx.x2;
        x.psi += dt * dx.psi;
        x.v_lon += dt * dx.v_lon;
        x.v_lat += dt * dx.v_lat;
        x.d_psi += dt * dx.d_psi;
        x.v = x.v_lon;
        dx.v = dx.v_lon;
    }
    x.delta += dt * dx.delta;
    x.delta = std::min(x.delta, limits.max_delta);
    x.delta = std::max(x.delta, -limits.max_delta);

    double acc_x = dx.v_lon - x.v_lat * x.d_psi;
    double acc_y = dx.v_lat + x.v_lon * x.d_psi;

    env::Spatz spatz(0, glm::vec3(x.x1, x.x2, x.psi));
    spatz.setVel(glm::vec3(dx.x1, dx.x2, 0));
    spatz.setSteerAngle(x.delta);
    spatz.setdRot(glm::vec3(0,0,x.d_psi));
    spatz.setAcc(glm::vec3(acc_x, acc_y, 0));
    spatz.alpha_front = alpha_front;
    spatz.alpha_rear = alpha_rear;

    return spatz;
}
