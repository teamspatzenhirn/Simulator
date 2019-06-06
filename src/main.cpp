#include "Loop.h"

int main (int argc, char* argv[]) {

    Settings settings;
    settings.load();

    if (argc > 1) {
        settings.configPath = std::string(argv[1]);
    }

    Scene scene(settings.configPath);

    Loop loop(800, 600, settings);
    loop.loop(scene);

    return 0;
}
