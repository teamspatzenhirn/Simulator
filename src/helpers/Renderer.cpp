#include "Renderer.h"

Renderer::Renderer(std::string title, GLuint width, GLuint height) {

    if (!glfwInit()) {
        std::cout << "Could not initialize GLFW!" << std::endl;
        std::exit(-1);
    }

    window = glfwCreateWindow(
        width, height, title.c_str(), nullptr, nullptr);

    glfwMakeContextCurrent(window);

    if (GLEW_OK != glewInit()) {
        std::cout << "GL Extension Wrangler initialization failed!" << std::endl;
        std::exit(-1);
    }
}

Renderer::~Renderer() {

    glfwDestroyWindow(window);
}

void Renderer::beginFrame() {

    beforeRender = std::chrono::system_clock::now();    
}

void Renderer::endFrame() { 

    glfwSwapBuffers(window);

    auto afterRender = std::chrono::system_clock::now();
    dt = std::chrono::duration_cast<std::chrono::milliseconds>(
        afterRender - beforeRender);
}
