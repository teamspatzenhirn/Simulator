#include "Loop.h"

std::shared_ptr<Loop> Loop::instance;

Loop::Loop(GLFWwindow* window, GLuint windowWidth, GLuint windowHeight)
    : window{window}
    , windowWidth{windowWidth}
    , windowHeight{windowHeight}
    , frameBuffer{windowWidth, windowHeight}
    , markerFrameBuffer{windowWidth, windowHeight}
    , screenQuad{windowWidth, windowHeight, "shaders/ScreenQuadFragment.glsl"}
    , screenQuadCar{windowWidth, windowHeight, "shaders/ScreenQuadCarFragment.glsl"}
    , guiModule{window} {

    fpsCamera.pose = glm::vec3(0.0f, 1.0f, -4.0f);
}

void Loop::framebufferSizeCallback(GLFWwindow* window, int width, int height) {

    instance->windowWidth = width;
    instance->windowHeight = height;

    instance->frameBuffer.resize(width, height);
    instance->markerFrameBuffer.resize(width, height);

    instance->fpsCamera.aspectRatio = ((float) width) / height;
}

void Loop::loop() {

    initInput(window);

    while (!glfwWindowShouldClose(window)) {

        timer.frameStep(); 

        updateInput();

        guiModule.begin();

        double deltaTime = 16;
        while (timer.updateStep(deltaTime)) {
            update(deltaTime);
        }

        glUseProgram(shaderProgram.id);

        // render fps / editor camera view if needed

        for(KeyEvent& e : getKeyEvents()) {
            if (e.key == GLFW_KEY_C && e.action == GLFW_PRESS) {
                fpsCameraActive = !fpsCameraActive;
            }
            if (e.key == GLFW_KEY_P && e.action == GLFW_PRESS) {
                scene.paused = !scene.paused;
            }
        }

        if (fpsCameraActive) {
            renderFpsView();
        }

        // render from main car camera perspective

        renderCarView();

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

            float carCameraAspect = scene.car.mainCamera.getAspectRatio();

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

        commModule.transmitMainCamera(scene.car, car.frameBuffer.id);
        commModule.transmitCar(scene.car);
        commModule.receiveVesc(scene.car.vesc);

        guiModule.renderRootWindow(scene);
        guiModule.renderCarPropertiesWindow(scene.car);
        guiModule.renderPoseWindow(markerModule.getSelection());
        guiModule.renderHelpWindow();
        guiModule.end();

        glfwSwapBuffers(window);
    }
}

void Loop::update(double deltaTime) {

    fpsCamera.update(window, deltaTime);

    car.update(scene.car, scene.paused, deltaTime);
}

void Loop::renderScene() {

    markerModule.add(light.pose);
    light.render(shaderProgram.id);

    car.render(scene.car, shaderProgram.id, markerModule);

    editor.renderScene(shaderProgram.id, scene.tracks);
}

void Loop::renderFpsView() {

    markerModule.update(window, fpsCamera);
    editor.updateInput(fpsCamera, scene.tracks, scene.groundSize);

    /*
     * TODO: update also fps camera position!
     */

    Scene preRenderScene = scene;
    update(timer.accumulator);

    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer.id);

    glViewport(0, 0, windowWidth, windowHeight);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    fpsCamera.render(shaderProgram.id);

    renderScene();

    // render marker overlay 

    glBindFramebuffer(GL_FRAMEBUFFER, markerFrameBuffer.id);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    markerModule.render(window, shaderProgram.id, fpsCamera);
    editor.renderMarkers(shaderProgram.id, scene.tracks);

    scene = preRenderScene;
}

void Loop::renderCarView() {

    Scene preRenderScene = scene;
    update(timer.accumulator);

    glBindFramebuffer(GL_FRAMEBUFFER, car.frameBuffer.id);

    glViewport(0, 0,
            scene.car.mainCamera.imageWidth,
            scene.car.mainCamera.imageHeight);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    car.mainCamera.render(shaderProgram.id);

    renderScene();

    scene = preRenderScene;
}
