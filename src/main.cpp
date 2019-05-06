#include <iostream>

#include "Loop.h"
#include "helpers/Input.h"

int main (int argc, char* argv[]) {

    int windowWidth = 800;
    int windowHeight = 600;

    // initialize OpenGL and GLEW

    if (!glfwInit()) {
        std::cout << "Could not initialize GLFW!" << std::endl;
        std::exit(-1);
    }

    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow* window = glfwCreateWindow(
        800, 600, "Spatz Simulator", nullptr, nullptr);

    glfwMakeContextCurrent(window);

    if (GLEW_OK != glewInit()) {
        std::cout << "GL Extension Wrangler initialization failed!" << std::endl;
        std::exit(-1);
    }

    // OpenGL and GLFW settings

    glClearColor(1.0, 1.0, 1.0, 1.0);

    glEnable(GL_DEPTH_TEST);

    glDepthFunc(GL_LEQUAL);

    glfwSwapInterval(0);

    // loop setup
    
    Settings settings;
    settings.load();

    if (argc > 1) {
        settings.configPath = std::string(argv[1]);
        std::cout << settings.configPath << std::endl;
    }

    Scene scene(settings.configPath);

    // entering main loop
    Loop loop(window, windowWidth, windowHeight, settings);
    loop.loop(scene);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
