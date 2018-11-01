#ifndef INC_2019_GUIMODULE_H
#define INC_2019_GUIMODULE_H

#include <vector>
#include <functional>

#include "Scene.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "helpers/Helpers.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class GuiModule {

    GLFWwindow* window;

    bool showCreateItemsWindow = true;
    bool showPoseWindow = true;
    bool showCarPropertiesWindow = false;
    bool showRulePropertiesWindow = true;
    bool showHelpWindow = false;

    std::string openedPath;
    std::string openedFilename;
    std::string currentDirectory;
    std::string selectedFilename;

    std::string errorMessage;

    void renderErrorDialog(std::string& msg);
    void renderOpenFileDialog(Scene& scene, bool show);
    void renderSaveFileDialog(Scene& scene, bool show, bool showSaveAs);
    void renderDirectoryListing();

    void renderCreateMenu(Scene& scene);
 
public:

    GuiModule(GLFWwindow* window, std::string scenePath);
    ~GuiModule();

    void renderRootWindow(Scene& scene);
    void renderPoseWindow(Pose* pose);
    void renderCarPropertiesWindow(Scene::Car& car);
    void renderRulePropertiesWindow(Scene::Rules& rules);
    void renderHelpWindow();
    void renderCreateDialog();

    void begin();
    void end();
};

#endif
