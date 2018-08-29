#ifndef INC_2019_GUIMODULE_H
#define INC_2019_GUIMODULE_H

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class GuiModule {

    GLFWwindow* window;

public:

    GuiModule(GLFWwindow* window);
    ~GuiModule();

    void begin();
    void end();
};

#endif
