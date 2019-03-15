#include "Loop.h"

std::shared_ptr<Loop> Loop::instance;

Loop::Loop(GLFWwindow* window, GLsizei windowWidth, GLsizei windowHeight, Settings settings)
    : window{window}
    , windowWidth{windowWidth}
    , windowHeight{windowHeight}
    , settings{settings}
    , scene{settings.configPath}
    , frameBuffer{windowWidth, windowHeight}
    , screenQuad{"shaders/ScreenQuadFragment.glsl"}
    , guiModule{window, settings.configPath} {
}

void Loop::framebufferSizeCallback(GLFWwindow* window, int width, int height) {

    if (Loop::instance->window == window) {

        instance->windowWidth = width;
        instance->windowHeight = height;

        instance->frameBuffer.resize(width, height);

        instance->scene.fpsCamera.aspectRatio = (float)width / (float)height;
    }
}

void renderToScreen (
        GLsizei windowWidth, 
        GLsizei windowHeight, 
        ScreenQuad& screenQuad, 
        float aspectRatio, 
        bool keepAspectRatio,
        GLuint textureId) {

    if (keepAspectRatio) {
        float fwidth = (float)windowWidth;
        float fheight = (float)windowHeight;

        if (fwidth / fheight > aspectRatio) {
            GLsizei width = (GLsizei)(fheight * aspectRatio);
            glViewport((windowWidth - width) / 2, 0, width, windowHeight);
        } else {
            GLsizei height = (GLsizei)(fwidth / aspectRatio);
            glViewport(0, (windowHeight - height) / 2, windowWidth, height);
        }
    } else { 
        glViewport(0, 0, windowWidth, windowHeight);
    }

    GLuint shaderProgramId = screenQuad.start();

    glUseProgram(shaderProgramId);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glUniform1i(glGetUniformLocation(shaderProgramId, "tex"), 0);

    screenQuad.end();
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
        guiModule.renderRuleWindow(scene.rules);
        guiModule.renderHelpWindow();


        float deltaTime = 0.004f;
        float simDeltaTime = deltaTime * settings.simulationSpeed;

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

        if (MAIN_CAMERA == selectedCamera) {
            renderToScreen(
                    windowWidth, 
                    windowHeight, 
                    screenQuad, 
                    scene.car.mainCamera.getAspectRatio(), 
                    true, 
                    car.frameBuffer.colorTextureId);
        } else if (DEPTH_CAMERA == selectedCamera) {
            renderToScreen(
                    windowWidth, 
                    windowHeight, 
                    screenQuad, 
                    scene.car.depthCamera.getDepthAspectRatio(), 
                    true, 
                    car.depthCameraFrameBuffer.colorTextureId);
        } else { // FPS_CAMERA
            renderToScreen(
                    windowWidth, 
                    windowHeight, 
                    screenQuad, 
                    1, 
                    false, 
                    frameBuffer.colorTextureId);
        }

        commModule.transmitMainCamera(scene.car, car.bayerFrameBuffer.id);
        commModule.transmitDepthCamera(scene.car, car.depthCameraFrameBuffer.id);

        guiModule.end();

        glfwSwapBuffers(window);
    }
}

void Loop::update(float deltaTime, float simDeltaTime) {

    scene.fpsCamera.update(window, deltaTime);

    updateCollisions();

    if (!scene.paused) {
        car.updatePosition(scene.car, simDeltaTime);
    }

    itemsModule.updateDynamicItems(simDeltaTime, scene.car, scene.dynamicItemSettings, scene.items);

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
        } else if (i->type == PEDESTRIAN) {
            collisionModule.add(i->pose, modelStore.itemModels[PEDESTRIAN]);
        } else if (i->type == DYNAMIC_PEDESTRIAN_RIGHT) {
            collisionModule.add(i->pose, modelStore.itemModels[PEDESTRIAN]);
        } else if (i->type == DYNAMIC_PEDESTRIAN_LEFT) {
            collisionModule.add(i->pose, modelStore.itemModels[PEDESTRIAN]);
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

    markerModule.render(shaderProgramId, scene.fpsCamera, scene.selection);
}

void Loop::renderFpsView() {

    if (settings.showMarkers) {
        markerModule.update(window, scene.fpsCamera, scene.selection);
        editor.updateInput(scene.fpsCamera, scene.tracks, scene.groundSize);
    }

    itemsModule.update(scene.items, scene.selection.pose);

    Scene preRenderScene = scene;
    update(timer.accumulator, timer.accumulator * settings.simulationSpeed);

    GLint timeLocation = glGetUniformLocation(shaderProgram.id, "time");
    glUniform1f(timeLocation, (float)scene.simulationTime * 1000);

    GLint noiseLocation = glGetUniformLocation(shaderProgram.id, "noise");
    glUniform1f(noiseLocation, 0.0f);

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

    visModule.renderDynamicItems(
            shaderProgram.id,
            scene.simulationTime,
            scene.items);

    visModule.renderVisualization(
            shaderProgram.id,
            scene.visualization, settings);

    scene = preRenderScene;
}

void Loop::renderCarView() {

    Scene preRenderScene = scene;
    update(timer.accumulator, timer.accumulator * settings.simulationSpeed);

    glUseProgram(carShaderProgram.id);

    GLint carShaderTimeLocation = 
        glGetUniformLocation(carShaderProgram.id, "time");
    glUniform1f(carShaderTimeLocation, (float)scene.simulationTime * 1000);

    GLint carShaderNoiseLocation = 
        glGetUniformLocation(carShaderProgram.id, "noise");
    glUniform1f(carShaderNoiseLocation, scene.car.mainCamera.noise);

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

        GLint timeLocation = glGetUniformLocation(shaderProgram.id, "time");
        glUniform1f(timeLocation, (float)scene.simulationTime * 1000);

        GLint noiseLocation = glGetUniformLocation(shaderProgram.id, "noise");
        glUniform1f(noiseLocation, scene.car.mainCamera.noise);

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
