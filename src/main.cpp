#include <chrono>
#include <iostream>

#include "helpers/Helpers.h" 
#include "Marker.h"

int main () {

    Renderer renderer(800, 600);

    Shader vertexShader("shaders/VertexShader.glsl", GL_VERTEX_SHADER);
    Shader fragmentShader("shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER);

    ShaderProgram shaderProgram(vertexShader, fragmentShader);

    // Marker markerModule;

    glUseProgram(shaderProgram.id);

    FpsCamera camera(45, 4.0f/3.0f);
    camera.view = glm::translate(camera.view, glm::vec3(0.0f, 0.0f, -4.0f));

    PointLight light(10.0f, 10.0f, 20.0f);

    glm::mat4 model = glm::mat4(1.0f);
    // markerModule.addMarker(model);
    model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));

    glClearColor(1.0, 1.0, 1.0, 1.0);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    Model cube("models/test_cube.obj");
    cube.upload();

    glfwSwapInterval(1);

    while (!glfwWindowShouldClose(renderer.window)) {
                
        renderer.beginFrame();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera.update(renderer.window, renderer.dt.count());
        camera.render(shaderProgram.id);

        light.render(shaderProgram.id);

        model = glm::rotate(model, 0.0025f, glm::vec3(0.0, 0.0f, 1.0f));

        cube.render(shaderProgram.id, model);

        // markerModule.render(renderer.window, shaderProgram.id, camera);

        //std::cout << renderer->dt.count() << std::endl;
        //std::cout << glGetError() << std::endl;

        renderer.endFrame();

        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}
