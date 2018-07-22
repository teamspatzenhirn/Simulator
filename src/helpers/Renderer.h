#ifndef INC_2019_RENDERER_H
#define INC_2019_RENDERER_H

#include <chrono>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Renderer {

    std::chrono::system_clock::time_point beforeRender;

public:

    std::chrono::milliseconds dt;

    GLFWwindow* window;

    Renderer(GLuint width, GLuint height);
    ~Renderer();

    void beginFrame();
    void endFrame();
};

#endif
