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

    bool showPoseWindow = true;
    bool showCarPropertiesWindow = false;

    std::map<std::string, bool*> showMenuItems;

    std::string openedPath;
    std::string openedFilename;
    std::string currentDirectory;
    std::string selectedFilename;

    std::string errorMessage;

    void renderErrorDialog(std::string& msg);
    void renderOpenFileDialog(Scene& scene, bool show);
    void renderSaveFileDialog(Scene& scene, bool show, bool showSaveAs);
    void renderDirectoryListing();
 
public:

    GuiModule(GLFWwindow* window);
    ~GuiModule();

    void renderRootWindow(Scene& scene);
    void renderPoseWindow(Pose* pose);
    void renderCarPropertiesWindow(Scene::Car& car);

    void begin();
    void end();

    void addShowMenuItem(std::string title, bool* show);
};

#endif
