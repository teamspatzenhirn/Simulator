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

    scene.load("default.json");
}

void Loop::framebufferSizeCallback(GLFWwindow* window, int width, int height) {

    instance->windowWidth = width;
    instance->windowHeight = height;

    instance->frameBuffer.resize(width, height);
    instance->markerFrameBuffer.resize(width, height);

    instance->scene.fpsCamera.aspectRatio = ((float) width) / height;
}

void Loop::loop() {

    initInput(window);

    while (!glfwWindowShouldClose(window)) {

        scene.addToHistory();

        timer.frameStep(); 

        updateInput();

        guiModule.begin();

        double deltaTime = 4.0f;
        double simDeltaTime = 1.0f;

        while (timer.updateStep(deltaTime)) {

            commModule.receiveVesc(scene.car.vesc);

            update(simDeltaTime);

            if (!scene.paused) {
                scene.simulationTime += simDeltaTime;
            }

            commModule.transmitCar(scene.car, scene.simulationTime);
        }

        ruleModule.update(
                scene.simulationTime,
                scene.rules,
                scene.car,
                scene.tracks,
                scene.items,
                collisionModule);

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

        commModule.transmitMainCamera(scene.car, car.bayerFrameBuffer.id);

        guiModule.renderRootWindow(scene);
        guiModule.renderCarPropertiesWindow(scene.car);
        guiModule.renderRulePropertiesWindow(scene.rules);
        guiModule.renderPoseWindow(markerModule.getSelection());
        guiModule.renderHelpWindow();
        guiModule.end();

        glfwSwapBuffers(window);
    }
}

void Loop::update(double deltaTime) {

    scene.fpsCamera.update(window, deltaTime);

    updateCollisions();

    if (!scene.paused) {
        car.updatePosition(scene.car, deltaTime);
    }

    car.updateMainCamera(scene.car);

}

void Loop::updateCollisions() {

    collisionModule.add(scene.car.modelPose, car.carModel);

    for (auto& i : scene.items) {
        if (i->type == OBSTACLE) {
            collisionModule.add(i->pose, itemsModule.obstacleModel);
        }
    }

    collisionModule.update();
}

void Loop::renderScene(GLuint shaderProgramId) {

    light.render(shaderProgramId);

    car.render(shaderProgramId, scene.car);

    itemsModule.render(shaderProgramId, scene.items);

    editor.renderScene(shaderProgramId, scene.tracks);
}

void Loop::renderMarkers(GLuint shaderProgramId) {

    markerModule.add(light.pose);
    markerModule.add(scene.car.modelPose);

    for (std::shared_ptr<Scene::Item>& i : scene.items) {
        markerModule.add(i->pose);
    }

    markerModule.render(window, shaderProgramId, scene.fpsCamera);
}

void Loop::renderFpsView() {

    markerModule.update(window, scene.fpsCamera);
    editor.updateInput(scene.fpsCamera, scene.tracks, scene.groundSize);
    itemsModule.update(scene.items, markerModule.getSelection());

    Scene preRenderScene = scene;
    update(timer.accumulator);

    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer.id);

    glViewport(0, 0, windowWidth, windowHeight);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    scene.fpsCamera.render(shaderProgram.id);

    renderScene(shaderProgram.id);

    // render marker overlay 

    glBindFramebuffer(GL_FRAMEBUFFER, markerFrameBuffer.id);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    editor.renderMarkers(shaderProgram.id, scene.tracks);
    renderMarkers(shaderProgram.id);

    scene = preRenderScene;
}

void Loop::renderCarView() {

    Scene preRenderScene = scene;
    update(timer.accumulator);

    glUseProgram(carShaderProgram.id);

    glBindFramebuffer(GL_FRAMEBUFFER, car.bayerFrameBuffer.id);

    glViewport(0, 0,
            scene.car.mainCamera.imageWidth,
            scene.car.mainCamera.imageHeight);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    car.mainCamera.render(carShaderProgram.id);

    renderScene(carShaderProgram.id);

    glUseProgram(shaderProgram.id);

    if (!fpsCameraActive) {

        glBindFramebuffer(GL_FRAMEBUFFER, car.frameBuffer.id);

        glViewport(0, 0,
                scene.car.mainCamera.imageWidth,
                scene.car.mainCamera.imageHeight);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        car.mainCamera.render(carShaderProgram.id);

        renderScene(carShaderProgram.id);
    }

    scene = preRenderScene;
}
