#ifndef INC_2019_GUIMODULE_H
#define INC_2019_GUIMODULE_H

#include <vector>
#include <functional>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

#include "Scene.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/ImGuiFileDialog.h"

#include "helpers/Helpers.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class GuiModule {

    GLFWwindow* window;

    std::map<std::string, bool*> showMenuItems;

    fs::path currentPath;
    fs::path selection;

    void renderRootWindow(Scene& scene);

    void renderOpenFileDialog(Scene& scene, bool show);
    void renderSaveFileDialog(Scene& scene, bool show);
    void renderDirectoryListing();

public:

    GuiModule(GLFWwindow* window);
    ~GuiModule();

    void begin();
    void end(Scene& scene);

    void addShowMenuItem(std::string title, bool* show);
};

#endif
