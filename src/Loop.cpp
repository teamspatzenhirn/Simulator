#include "Loop.h"

std::shared_ptr<Loop> Loop::instance;

Loop::Loop(GLFWwindow* window, GLuint windowWidth, GLuint windowHeight, Settings settings)
    : window{window}
    , windowWidth{windowWidth}
    , windowHeight{windowHeight}
    , settings{settings}
    , scene{settings.configPath}
    , frameBuffer{windowWidth, windowHeight}
    , screenQuad{windowWidth, windowHeight, "shaders/ScreenQuadFragment.glsl"}
    , guiModule{window, settings.configPath} {
}

void Loop::framebufferSizeCallback(GLFWwindow* window, int width, int height) {

    instance->windowWidth = width;
    instance->windowHeight = height;

    instance->frameBuffer.resize(width, height);

    instance->scene.fpsCamera.aspectRatio = ((float) width) / height;
}

void Loop::loop() {

    initInput(window);

    while (!glfwWindowShouldClose(window)) {

        scene.addToHistory();

        timer.frameStep(); 

        updateInput();

        // TODO: make this less hacky, this is not right here

        for (MouseButtonEvent& evt : getMouseButtonEvents()) {
            if (evt.action == GLFW_PRESS && evt.button == GLFW_MOUSE_BUTTON_LEFT) {
                scene.selection.handled = false;
            }
        }

        guiModule.begin();

        guiModule.renderRootWindow(scene, settings);
        guiModule.renderSceneWindow(scene);
        guiModule.renderSettingsWindow(settings);
        guiModule.renderHelpWindow();

        double deltaTime = 4.0f;
        double simDeltaTime = deltaTime * settings.simulationSpeed;

        while (timer.updateStep(deltaTime)) {

            commModule.receiveVesc(scene.car.vesc);
            commModule.receiveVisualization(scene.visualization);

            update(deltaTime, simDeltaTime);

            if (!scene.paused) {
                scene.simulationTime += simDeltaTime;
            }

            commModule.transmitCar(scene.car, scene.paused, scene.simulationTime);
        }

        ruleModule.update(
                scene.simulationTime,
                scene.rules,
                scene.car,
                scene.tracks,
                scene.items,
                collisionModule);

        glUseProgram(shaderProgram.id);

        for(KeyEvent& e : getKeyEvents()) {
            if (e.key == GLFW_KEY_C && e.action == GLFW_PRESS) {
                selectedCamera = (SelectedCamera)((((int)selectedCamera) + 1) % 3);
            }
            if (e.key == GLFW_KEY_P && e.action == GLFW_PRESS) {
                scene.paused = !scene.paused;
            }
        }

        // render fps / editor camera view if needed

        if (FPS_CAMERA == selectedCamera) {
            renderFpsView();
        }

        // render from main car camera perspective

        renderCarView();

        // render from depth camera perspective

        renderDepthView();

        // render on screen filling quad

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        if (FPS_CAMERA == selectedCamera) {

            glViewport(0, 0, windowWidth, windowHeight);

            GLuint shaderProgramId = screenQuad.start();

            glUseProgram(shaderProgramId);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, frameBuffer.colorTextureId);
            glUniform1i(glGetUniformLocation(shaderProgramId, "tex"), 0);

            screenQuad.end();
            
        } else if (MAIN_CAMERA == selectedCamera) {

            float carCameraAspect = scene.car.mainCamera.getAspectRatio();

            if (((float) windowWidth) / windowHeight > carCameraAspect) {
                int width = windowHeight * carCameraAspect;
                glViewport((windowWidth - width) / 2, 0, width, windowHeight);
            } else {
                int height = windowWidth / carCameraAspect;
                glViewport(0, (windowHeight - height) / 2, windowWidth, height);
            }

            GLuint shaderProgramId = screenQuad.start();

            glUseProgram(shaderProgramId);

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, car.frameBuffer.colorTextureId);
            glUniform1i(glGetUniformLocation(shaderProgramId, "tex"), 0);

            screenQuad.end();

        } else if (DEPTH_CAMERA == selectedCamera) {

            float carCameraAspect = scene.car.depthCamera.getDepthAspectRatio();

            if (((float) windowWidth) / windowHeight > carCameraAspect) {
                int width = windowHeight * carCameraAspect;
                glViewport((windowWidth - width) / 2, 0, width, windowHeight);
            } else {
                int height = windowWidth / carCameraAspect;
                glViewport(0, (windowHeight - height) / 2, windowWidth, height);
            }

            GLuint shaderProgramId = screenQuad.start();

            glUseProgram(shaderProgramId);

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, car.depthCameraFrameBuffer.colorTextureId);
            glUniform1i(glGetUniformLocation(shaderProgramId, "tex"), 0);

            screenQuad.end();
        }

        commModule.transmitMainCamera(scene.car, car.bayerFrameBuffer.id);
        commModule.transmitDepthCamera(scene.car, car.depthCameraFrameBuffer.id);

        guiModule.end();

        glfwSwapBuffers(window);
    }
}

void Loop::update(double deltaTime, double simDeltaTime) {

    scene.fpsCamera.update(window, deltaTime);

    updateCollisions();

    if (!scene.paused) {
        car.updatePosition(scene.car, simDeltaTime);
    }

    itemsModule.updateDynamicObstacles(simDeltaTime, scene.car, scene.items);

    visModule.addPositionTrace(scene.car.modelPose.position, scene.simulationTime);

    car.updateMainCamera(scene.car.mainCamera, scene.car.modelPose);
    car.updateDepthCamera(scene.car.depthCamera, scene.car.modelPose);
    car.updateLaserSensors(scene.car, modelStore, scene.items);
}

void Loop::updateCollisions() {

    collisionModule.add(scene.car.modelPose, car.carModel);

    for (auto& i : scene.items) {
        if (i->type == OBSTACLE) {
            collisionModule.add(i->pose, modelStore.itemModels[OBSTACLE]);
        } else if (i->type == DYNAMIC_OBSTACLE) {
            collisionModule.add(i->pose, modelStore.itemModels[DYNAMIC_OBSTACLE]);
        }
    }

    collisionModule.update();
}

void Loop::renderScene(GLuint shaderProgramId) {

    light.render(shaderProgramId);

    car.render(shaderProgramId, scene.car);

    itemsModule.render(shaderProgramId, modelStore, scene.items);

    editor.renderScene(shaderProgramId, scene.tracks);
}

void Loop::renderMarkers(GLuint shaderProgramId) {

    markerModule.add(light.pose, MarkerModule::TRANSLATE_ALL);
    markerModule.add(scene.car.modelPose,
            MarkerModule::TRANSLATE_X 
            | MarkerModule::TRANSLATE_Z
            | MarkerModule::ROTATE_Y);

    for (std::shared_ptr<Scene::Item>& i : scene.items) {
        markerModule.add(i->pose,
                MarkerModule::TRANSLATE_X
                | MarkerModule::TRANSLATE_Z
                | MarkerModule::SCALE_ALL
                | MarkerModule::ROTATE_Y);
    }

    markerModule.render(window, shaderProgramId, scene.fpsCamera, scene.selection);
}

void Loop::renderFpsView() {

    if (settings.showMarkers) {
        markerModule.update(window, scene.fpsCamera, scene.selection);
        editor.updateInput(scene.fpsCamera, scene.tracks, scene.groundSize);
    }

    itemsModule.update(scene.items, scene.selection.pose);

    Scene preRenderScene = scene;
    update(timer.accumulator, timer.accumulator * settings.simulationSpeed);

    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer.id);

    glViewport(0, 0, windowWidth, windowHeight);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    scene.fpsCamera.render(shaderProgram.id);

    renderScene(shaderProgram.id);

    // render markers over everything else
    // thus we clean the depth buffer here

    glClear(GL_DEPTH_BUFFER_BIT);

    if (settings.showMarkers) {
        editor.renderMarkers(
                shaderProgram.id,
                scene.tracks,
                scene.fpsCamera.pose.position);
        renderMarkers(shaderProgram.id);
    }

    if (settings.showVehiclePath) {
        visModule.renderPositionTrace(
                shaderProgram.id,
                scene.simulationTime,
                settings.fancyVehiclePath);
    }

    visModule.renderSensors(
            shaderProgram.id,
            scene.car,
            settings);

    visModule.renderVisualization(
            shaderProgram.id,
            scene.visualization, settings);

    scene = preRenderScene;
}

void Loop::renderCarView() {

    Scene preRenderScene = scene;
    update(timer.accumulator, timer.accumulator * settings.simulationSpeed);

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

    if (MAIN_CAMERA == selectedCamera) {

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

void Loop::renderDepthView() {

    Scene preRenderScene = scene;

    update(timer.accumulator, timer.accumulator * settings.simulationSpeed);

    glUseProgram(depthCameraShaderProgram.id);

    glBindFramebuffer(GL_FRAMEBUFFER, car.depthCameraFrameBuffer.id);

    glViewport(0, 0,
            scene.car.depthCamera.depthImageWidth,
            scene.car.depthCamera.depthImageHeight);

    glClearColor(1.0f, 1.0f, 100.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    car.depthCamera.render(depthCameraShaderProgram.id);

    renderScene(depthCameraShaderProgram.id);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    scene = preRenderScene;
}
