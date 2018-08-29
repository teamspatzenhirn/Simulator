#ifndef SPATZ_SIM_H
#define SPATZ_SIM_H

#include <cmath>

#include "Spatz.h"

struct WheelParams {
    double B_front, B_rear;
    double C_front, C_rear;
    double D_front, D_rear;
    double k_front, k_rear;
};

struct Limits { // Stellgrößenbeschränkungen
    double max_F, max_delta, max_d_delta;
};

struct ControlVarsVESC {
    // Stellgrößen für den Vedder-Regler (VESC)
    double vel; // Geschwindigkeit
    double delta; // Lenkwinkel
};

struct X { // Zustand
    double x1, x2, psi, delta, v, v_lon, v_lat, d_psi;
};

class SpatzSimulator {

public:

    SpatzSimulator();

    void reset();
    void setSystemParams(env::Spatz::SystemParams system);
    void setWheelParams(WheelParams wheelParams);
    void setLimits(Limits limits);

    env::Spatz step(ControlVarsVESC ctrlVars, double dt, bool pacejkaModel);

protected:

    env::Spatz::SystemParams sys;
    WheelParams wheelParams;
    Limits limits;
    X x;
};

#endif
