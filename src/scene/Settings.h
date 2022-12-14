#ifndef INC_2019_SETTINGS_H
#define INC_2019_SETTINGS_H

#include <string>

/* 
 * This contains settings that are not stored per config, 
 * but as a (usually) global settings file.
 */
struct Settings {

    /*
     * The path where the global settings file is stored.
     */
    std::string settingsFilePath;

    /*
     * The path to the config file that will be loaded on startup.
     */
    std::string configPath;

    /*
     * The path from where resources (e.g. models, shaders) are loaded.
     */
    std::string resourcePath = "/usr/local/share/spatzsim/";

    /*
     * Dimensions of the main window.
     */
    int windowWidth = 800;
    int windowHeight = 600;

    /*
     * The amount of samples used for multi sample antialiasing in the editor
     * view. Must be > 0 and <= to what your GPU can handle.
     */
    int msaaSamplesEditorView = 4;

    /*
     * Whether to use fullscreen mode for the main window.
     */
    bool fullscreen = false;

    /*
     * The speed of the simulation given as fraction of real time.
     */
    float simulationSpeed = 0.25f;

    /*
     * The delta time (in seconds) for one simulation update. 
     */
    float updateDeltaTime = 0.005f;

    /*
     * If set marker/modifier points will be rendered.
     */
    bool showMarkers = true;

    /*
     * If set the path of the vehicle, that is the position
     * history, will be draw.
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
     * If set, the ray sent out by the front laser sensor will be draw.
     */
    bool showLaserSensor = true;

    /*
     * If set, the ray sent out by the back binary light sensor will be draw.
     */
    bool showBinaryLightSensor = true;

    /*
     * If set, the simulator will instantly close in autotrack mode
     * once any rule was broken.
     */
    bool instantCloseInAutotrack = true;
};

#endif
