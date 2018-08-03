#include <iostream>

#include "Loop.h"

int main () {

    int windowWidth = 800;
    int windowHeight = 600;

    // initialize OpenGL and GLEW

    if (!glfwInit()) {
        std::cout << "Could not initialize GLFW!" << std::endl;
        std::exit(-1);
    }

    GLFWwindow* window = glfwCreateWindow(
        windowWidth, windowHeight, "Spatz Simulator", nullptr, nullptr);

    glfwMakeContextCurrent(window);

    if (GLEW_OK != glewInit()) {
        std::cout << "GL Extension Wrangler initialization failed!" << std::endl;
        std::exit(-1);
    }
    
    // OpenGL and GLFW settings

    glClearColor(1.0, 1.0, 1.0, 1.0);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glfwSwapInterval(1);

    // loop setup

    std::shared_ptr<Loop> loop = std::make_shared<Loop>(window, windowWidth, windowHeight);

    Loop::instance = loop;

    glfwSetFramebufferSizeCallback(window, Loop::framebufferSizeCallback);
    glfwSetKeyCallback(window, Loop::keyCallback);

    // main loop
    loop->loop();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
