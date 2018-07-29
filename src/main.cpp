#include <chrono>
#include <iostream>

#include "helpers/Helpers.h" 
#include "modules/MarkerModule.h"

int main () {

    Renderer renderer("Spatz Simulator", 800, 600);

    Shader vertexShader("shaders/VertexShader.glsl", GL_VERTEX_SHADER);
    Shader fragmentShader("shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER);

    ShaderProgram shaderProgram(vertexShader, fragmentShader);
    glUseProgram(shaderProgram.id);

    FrameBuffer frameBuffer(800, 600);

    MarkerModule markerModule;

    ScreenQuad screenQuad(800, 600, "shaders/ScreenQuadFragment.glsl");

    FpsCamera camera(45, 4.0f/3.0f);
    camera.view = glm::translate(camera.view, glm::vec3(0.0f, 0.0f, -4.0f));

    PointLight light(10.0f, 10.0f, 20.0f);

    glm::mat4 modelMat = glm::mat4(1.0f);
    markerModule.addMarker(modelMat);
    modelMat = glm::scale(modelMat, glm::vec3(0.4f, 0.4f, 1.0f));

    glClearColor(1.0, 1.0, 1.0, 1.0);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    Model cube("models/test_cube.obj");
    cube.upload();

    glfwSwapInterval(1);

    while (!glfwWindowShouldClose(renderer.window)) {

        glfwPollEvents();

        renderer.beginFrame();

        // actual scene rendered to framebuffer
        
        glUseProgram(shaderProgram.id);

        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer.id);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera.update(renderer.window, renderer.dt.count());
        camera.render(shaderProgram.id);

        light.render(shaderProgram.id);

        modelMat = glm::rotate(
                modelMat,
                0.0002f * renderer.dt.count(),
                glm::vec3(0.0, 0.0f, 1.0f));

        cube.render(shaderProgram.id, modelMat);

        markerModule.render(renderer.window, shaderProgram.id, camera);
        
        // render on screen quad

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        screenQuad.render([&](){

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, frameBuffer.colorTextureId);
            glUniform1i(glGetUniformLocation(screenQuad.shaderProgram->id, "tex"), 0);
        });

        renderer.endFrame();

        //std::cout << renderer.dt.count() << std::endl;
        //std::cout << glGetError() << std::endl;
    }

    glfwTerminate();

    return 0;
}
