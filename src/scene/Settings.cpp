#include "Settings.h"

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
}
