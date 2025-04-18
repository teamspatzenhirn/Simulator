#ifndef INC_2019_GUIMODULE_H
#define INC_2019_GUIMODULE_H

#include "scene/Scene.h"
#include "scene/Settings.h"

class GuiModule {

    GLFWwindow* window;

    bool showSceneWindow = false;
    bool showSettingsWindow = false;
    bool showRuleWindow = false;
    bool showHelpWindow = false;
    bool showAboutWindow = false;

    std::string imguiIniPath;
    std::string openedFilePath;
    std::string selectedFilePath;

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
    bool renderSettingsWindow(Settings& settings);
    void renderRuleWindow(const Scene::Rules& rules);
    void renderHelpWindow();
    void renderAboutWindow();

    void begin();
    void end();
};

#endif
