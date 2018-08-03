#include <iostream>

#include "Loop.h"

std::shared_ptr<Loop> Loop::instance;

Loop::Loop(GLFWwindow* window, GLuint windowWidth, GLuint windowHeight)
    : window{window}
    , windowWidth{windowWidth}
    , windowHeight{windowHeight}
    , frameBuffer{windowWidth, windowHeight}
    , markerFrameBuffer{windowWidth, windowHeight}
    , screenQuad{windowWidth, windowHeight, "shaders/ScreenQuadFragment.glsl"}
    , screenQuadCar{windowWidth, windowHeight, "shaders/ScreenQuadCarFragment.glsl"} {

    camera.view = glm::translate(camera.view, glm::vec3(0.0f, 0.0f, -4.0f));

    markerModule.addMarker(modelMat);
    modelMat = glm::scale(modelMat, glm::vec3(0.4f, 0.4f, 1.0f));

    cube.upload();
}

void Loop::framebufferSizeCallback(GLFWwindow* window, int width, int height) {

    instance->windowWidth = width;
    instance->windowHeight = height;

    instance->frameBuffer.resize(width, height);
    instance->markerFrameBuffer.resize(width, height);

    instance->camera.setAspectRatio(((float) width) / height);
}

void Loop::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        instance->fpsCameraActive = !instance->fpsCameraActive;
    }
}

void Loop::loop() {

    while (!glfwWindowShouldClose(window)) {

        timer.beginFrame();

        glfwPollEvents();

        // update model

        modelMat = glm::rotate(
                modelMat,
                0.0002f * timer.dt.count(),
                glm::vec3(0.0, 0.0f, 1.0f));

        // render

        glUseProgram(shaderProgram.id);

        // render scene for fps camera

        if (fpsCameraActive) {
            // actual scene rendered to framebuffer

            glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer.id);

            glViewport(0, 0, windowWidth, windowHeight);

            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            camera.update(window, timer.dt.count());

            camera.render(shaderProgram.id);

            light.render(shaderProgram.id);

            cube.render(shaderProgram.id, modelMat);

            // render marker overlay 

            glBindFramebuffer(GL_FRAMEBUFFER, markerFrameBuffer.id);

            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            markerModule.render(window, shaderProgram.id, camera);
        }

        // render scene for car camera

        glBindFramebuffer(GL_FRAMEBUFFER, car.frameBuffer.id);

        glViewport(0, 0, carCameraWidth, carCameraHeight);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        car.camera.render(shaderProgram.id);

        light.render(shaderProgram.id);

        cube.render(shaderProgram.id, modelMat);

        // render on screen filling quad

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        if (fpsCameraActive) {

            glViewport(0, 0, windowWidth, windowHeight);

            // this is stupid!
            // i really don't like doing this with a callback

            screenQuad.render([&](GLuint shaderProgramId){

                glUseProgram(shaderProgramId);

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, frameBuffer.colorTextureId);
                glUniform1i(glGetUniformLocation(shaderProgramId, "tex0"), 0);

                glActiveTexture(GL_TEXTURE0 + 1);
                glBindTexture(GL_TEXTURE_2D, markerFrameBuffer.colorTextureId);
                glUniform1i(glGetUniformLocation(shaderProgramId, "tex1"), 1);
            });
        } else {
            if (((float) windowWidth) / windowHeight > carCameraAspect) {
                int width = windowHeight * carCameraAspect;
                glViewport((windowWidth - width) / 2, 0, width, windowHeight);
            } else {
                int height = windowWidth / carCameraAspect;
                glViewport(0, (windowHeight - height) / 2, windowWidth, height);
            }

            screenQuadCar.render([&](GLuint shaderProgramId){

                glUseProgram(shaderProgramId);

                glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, car.frameBuffer.colorTextureId);
                glUniform1i(glGetUniformLocation(shaderProgramId, "tex"), 0);
            });
        }

        glfwSwapBuffers(window);

        timer.endFrame();

        //std::cout << timer.dt.count() << std::endl;
        //std::cout << glGetError() << std::endl;
    }
}
