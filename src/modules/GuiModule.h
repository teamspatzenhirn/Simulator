#ifndef INC_2019_GUIMODULE_H
#define INC_2019_GUIMODULE_H

#include <vector>
#include <functional>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "helpers/Helpers.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class GuiModule {

    GLFWwindow* window;

    struct ShowMenuItem {
        std::string title;
        bool* show;
    };
    std::vector<ShowMenuItem> showMenuItems;

    void renderRootWindow();

public:

    GuiModule(GLFWwindow* window);
    ~GuiModule();

    void begin();
    void end();

    void addShowMenuItem(std::string title, bool* show);
};

#endif
