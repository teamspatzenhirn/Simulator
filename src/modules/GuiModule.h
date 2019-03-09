#ifndef INC_2019_GUIMODULE_H
#define INC_2019_GUIMODULE_H

#include <vector>
#include <functional>
#include <algorithm>

#include "Scene.h"

#include "ocornut_imgui/imgui.h"
#include "ocornut_imgui/examples/imgui_impl_glfw.h"
#include "ocornut_imgui/examples/imgui_impl_opengl3.h"

#include "helpers/Helpers.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

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
