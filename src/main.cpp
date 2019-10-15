#include "Loop.h"
#include "Storage.h"
#include "p-ranav_argparse/argparse.hpp"

int main (int argc, char* argv[]) {

    // Start by preparing and parsing arguments

    argparse::ArgumentParser parser("spatzsim");

    parser.add_argument("-c", "--config")
        .nargs(1)
        .default_value(std::string(""))
        .help("path to a config json file to load at startup");

    parser.add_argument("-s", "--settings")
        .nargs(1)
        .default_value(std::string(""))
        .help("path to the settings json file to be used");

    parser.add_argument("-r", "--resources")
        .nargs(1)
        .default_value(std::string(""))
        .help("path to an alternative resource directory (containing models, shaders)");

    parser.add_argument("-p", "--pause")
        .default_value(false)
        .implicit_value(true)
        .help("pause simulation timer at startup");

    try {
        parser.parse_args(argc, argv);
    } catch (const std::runtime_error& err) {
        if (err.what() != std::string("help called")) {
            std::cout << err.what() << "\n" << std::endl;
            parser.print_help();
            return -1;
        } 
        parser.print_help();
        return 0;
    }

    std::string argConfigPath = parser.get<std::string>("-c");
    std::string argSettingsPath = parser.get<std::string>("-s");
    std::string argResourcePath = parser.get<std::string>("-r");
    bool argPauseOnStartup = parser.get<bool>("-p");

    // Setting up objects, initiating main loop

    storage::createXDGSettingsDirectory();

    Settings settings;

    bool settingsOk;
    std::string settingsPath;

    if (argSettingsPath.empty()) {
        settingsOk = storage::load(settings);
        settingsPath = settings.settingsFilePath;
    } else {
        settingsOk = storage::load(settings, argSettingsPath);
        settingsPath = argSettingsPath;
    }

    if (!settingsOk) { 
        std::cerr << "No settings found at "
                  << settingsPath
                  << " failed!"
                  << std::endl;
        std::cerr << "Using default settings." << std::endl;
    }

    storage::save(settings);

    if (!argConfigPath.empty()) {
        settings.configPath = argConfigPath;
    }

    Scene scene(settings.configPath);
    scene.paused = argPauseOnStartup;

    Loop loop(settings);
    loop.loop(scene);

    return 0;
}
