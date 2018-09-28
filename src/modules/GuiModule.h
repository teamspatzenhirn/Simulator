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

    std::map<std::string, bool*> showMenuItems;

    std::string openedPath;
    std::string openedFilename;
    std::string currentDirectory;
    std::string selectedFilename;

    void renderRootWindow(Scene& scene);

    void renderOpenFileDialog(Scene& scene, bool show);
    void renderSaveFileDialog(Scene& scene, bool show, bool showSaveAs);
    void renderDirectoryListing();

public:

    GuiModule(GLFWwindow* window);
    ~GuiModule();

    void begin();
    void end(Scene& scene);

    void addShowMenuItem(std::string title, bool* show);
};

#endif
