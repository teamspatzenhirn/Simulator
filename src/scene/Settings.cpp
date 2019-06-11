#include "Settings.h"

#include <fstream>
#include <iostream>

#include "nlohmann_json/json.hpp"

using json = nlohmann::json;

template <typename T>
bool tryGet(const json& j, std::string name, T& variable) {

    try {
        variable = j.at(name).get<T>();
    } catch(std::exception& e) {
        json err(variable);
        std::cout << "Property \""
            << name
            << "\" not found. Using default: "
            << err.dump(4)
            << std::endl;
        return false;
    }

    return true;
}

void to_json(json& j, const Settings& s) {

    j = json({
            {"windowWidth", s.windowWidth},
            {"windowHeight", s.windowHeight},
            {"simulationSpeed", s.simulationSpeed},
            {"configPath", s.configPath},
            {"showMarkers", s.showMarkers},
            {"showVehiclePath", s.showVehiclePath},
            {"fancyVehiclePath", s.fancyVehiclePath},
            {"showVehicleTrajectory", s.showVehicleTrajectory},
            {"showLaserSensor", s.showLaserSensor},
            {"showBinaryLightSensor", s.showBinaryLightSensor}
        });
}

void from_json(const json& j, Settings& s) {

    tryGet(j, "windowWidth", s.windowWidth);
    tryGet(j, "windowHeight", s.windowHeight);
    tryGet(j, "simulationSpeed", s.simulationSpeed);
    tryGet(j, "configPath", s.configPath);
    tryGet(j, "showMarkers", s.showMarkers);
    tryGet(j, "showVehiclePath", s.showVehiclePath);
    tryGet(j, "fancyVehiclePath", s.fancyVehiclePath);
    tryGet(j, "showVehicleTrajectory", s.showVehicleTrajectory);
    tryGet(j, "showLaserSensor", s.showLaserSensor);
    tryGet(j, "showBinaryLightSensor", s.showBinaryLightSensor);
}

Settings::Settings() {

    std::string strHomePath(".");
    char* homePath = std::getenv("HOME");

    if (homePath) {
        strHomePath = std::string(homePath);
    } else {
        homePath = std::getenv("HOMEPATH");
        if (homePath) {
            strHomePath = std::string(homePath);
        }
    }

    settingsFilePath = strHomePath + "/.spatzsim";

    resourcePath = "../";
    char* envResPath = std::getenv("SPATZSIM_RESOURCE_PATH");

    if (envResPath) {
        resourcePath = std::string(envResPath);
    }

    load();
}

bool Settings::save() {

    std::ofstream out(settingsFilePath);

    if (!out) {
        return false;
    }

    out << json(*this).dump(4);
    out.close();

    return true;
}

bool Settings::load() {

    std::ifstream in(settingsFilePath);

    if (!in) {
        return false;
    }

    try {
        json j;
        in >> j;
        *this = j;
    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
        return false;
    }

    in.close();

    return true;
}
