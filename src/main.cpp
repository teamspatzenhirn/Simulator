#include "Loop.h"
#include "Storage.h"

int main (int argc, char* argv[]) {

    createResourcePath();

    Settings settings;
    if (!load(settings)) { 
        std::cerr << "Loading settings from "
                  << getResourcePath()
                  << "settings.json failed!"
                  << std::endl;
    }

    if (argc > 1) {
        settings.configPath = std::string(argv[1]);
    }

    Scene scene(settings.configPath);

    Loop loop(settings);
    loop.loop(scene);

    return 0;
}
