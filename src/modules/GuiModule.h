#ifndef INC_2019_GUIMODULE_H
#define INC_2019_GUIMODULE_H

#include "Scene.h"

class GuiModule {

    GLFWwindow* window;

    bool showCreateItemsWindow = true;
    bool showSceneWindow = true;
    bool showSettingsWindow = false;
    bool showHelpWindow = false;

    std::string openedPath;
    std::string openedFilename;
    std::string currentDirectory;
    std::string selectedFilename;

    std::string errorMessage;

    void renderErrorDialog(std::string& msg);
    void renderOpenFileDialog(Scene& scene, Settings& settings, bool show);
    void renderSaveFileDialog(Scene& scene, bool show, bool showSaveAs);
    void renderDirectoryListing();

    void renderCreateMenu(Scene& scene);
    void renderPoseGui(Pose& pose);
 
public:

    GuiModule(GLFWwindow* window, std::string scenePath);
    ~GuiModule();

    void renderRootWindow(Scene& scene, Settings& settings);
    void renderSceneWindow(Scene& scene);
    void renderSettingsWindow(Settings& settings);
    void renderHelpWindow();

    void begin();
    void end();
};

#endif
