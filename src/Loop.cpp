#include "Loop.h"

void setFramebufferSizeCallback(GLFWwindow* window, int width, int height) {

    Loop& loop = *(Loop*)glfwGetWindowUserPointer(window);
    loop.setFramebufferSize(window, width, height);
}

Loop::Loop(GLFWwindow* window, GLsizei windowWidth, GLsizei windowHeight, Settings settings)
    : window{window}
    , windowWidth{windowWidth}
    , windowHeight{windowHeight}
    , frameBuffer{windowWidth, windowHeight}
    , settings{settings}
    , screenQuad{
        settings.resourcePath + "shaders/ScreenQuadVertex.glsl",
        settings.resourcePath + "shaders/ScreenQuadFragment.glsl"}
    , fpsShaderProgram{
        settings.resourcePath + "shaders/VertexShader.glsl", 
        settings.resourcePath + "shaders/FragmentShader.glsl"}
    , carShaderProgram{
        settings.resourcePath + "shaders/BayerVertexShader.glsl", 
        settings.resourcePath + "shaders/BayerFragmentShader.glsl"}
    , depthCameraShaderProgram{
        settings.resourcePath + "shaders/BayerVertexShader.glsl", 
        settings.resourcePath + "shaders/DepthPointsFragmentShader.glsl"}
    , modelStore{settings.resourcePath}
    , guiModule{window, settings.configPath} {

    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, setFramebufferSizeCallback);

    initInput(window);
}

void renderToScreen (
        GLsizei windowWidth, 
        GLsizei windowHeight, 
        ScreenQuad& screenQuad, 
        float aspectRatio, 
        bool keepAspectRatio,
        GLuint textureId) {

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

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

void Loop::loop(Scene& scene) {

    auto time = std::chrono::steady_clock::now();

    while (!glfwWindowShouldClose(window)) {

        auto now = std::chrono::steady_clock::now();

        float frameDeltaTime = 
            (float)std::chrono::duration_cast<std::chrono::microseconds>(
                    now - time).count() / 1000000.0f;
        time = now;

        scene.displayClock.windup(frameDeltaTime); 
        if (!scene.paused) {
            scene.simulationClock.windup(frameDeltaTime * settings.simulationSpeed); 
        }

        step(scene, frameDeltaTime);
    }
}

void Loop::step(Scene& scene, float frameDeltaTime) {

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

    // gui updates 

    while (scene.displayClock.step(settings.updateDeltaTime)) {

        commModule.receiveVisualization(scene.visualization);

        if (FPS_CAMERA == selectedCamera) {
            scene.fpsCamera.update(window, settings.updateDeltaTime);
        } else if (FOLLOW_CAMERA == selectedCamera) {
            scene.followCamera.update(window, settings.updateDeltaTime, 
                    scene.car.modelPose);
        }
    }

    for(KeyEvent& e : getKeyEvents()) {
        if (e.key == GLFW_KEY_C && e.action == GLFW_PRESS) {
            selectedCamera = (SelectedCamera)((((int)selectedCamera) + 1) % 3);
        }
        if (e.key == GLFW_KEY_P && e.action == GLFW_PRESS) {
            scene.paused = !scene.paused;
        }
    }

    if (FPS_CAMERA == selectedCamera) {

        scene.fpsCamera.aspectRatio = (float)windowWidth / (float)windowHeight;

        if (settings.showMarkers) {
            markerModule.update(window, scene.fpsCamera, scene.selection);
            editor.updateInput(scene.fpsCamera, scene.tracks, scene.groundSize);
        }

        itemsModule.update(scene.items, scene.selection.pose);
    }

    // actual simulation updates
    
    while (scene.simulationClock.step(settings.updateDeltaTime)) {

        // TODO: Doing receive in such a way is not really correct!
        // Likely the vesc value will not actually change n-times
        // during the iteration. Probably, we will read the same
        // value n-times. We need to make sure that the buffer
        // queue in the shared memory is actually used.

        commModule.receiveVesc(scene.car.vesc);

        update(scene, settings.updateDeltaTime);

        ruleModule.update(
                scene.simulationClock.time,
                scene.rules,
                scene.car,
                scene.tracks,
                scene.items,
                collisionModule);

        commModule.transmitCar(
                scene.car, 
                scene.paused, 
                scene.simulationClock.time);
    }

    // render camera images
    
    scene.addToHistory();

    Scene preRenderScene = scene;

    update(scene, scene.simulationClock.accumulator);

    if (FPS_CAMERA == selectedCamera) {
        scene.fpsCamera.update(window, scene.displayClock.accumulator);
    } else if (FOLLOW_CAMERA == selectedCamera) {
        scene.followCamera.update(window, scene.displayClock.accumulator, 
                scene.car.modelPose);
    }

    renderCarView(scene);
    renderDepthView(scene);
    renderFpsView(scene);

    // render on screen filling quad

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
    } else { // FPS_CAMERA or FOLLOW_CAMERA
        renderToScreen(
                windowWidth, 
                windowHeight, 
                screenQuad, 
                1, 
                false, 
                frameBuffer.colorTextureId);
    }

    scene = preRenderScene;

    guiModule.end();

    commModule.transmitMainCamera(scene.car, car.bayerFrameBuffer.id);
    commModule.transmitDepthCamera(scene.car, car.depthCameraFrameBuffer.id);

    glfwSwapBuffers(window);

    // check if lock request if yes ->
    //

    /*

    sim = SpatzSim()

    with Scene(sim) as scene:
        scene.settings.asdf
        scene.load("config")
        scene.get_previous_frame()
        scene.step(0.05) // simulator should advance 0.05 seconds

    sim.run()

    */
}

void Loop::update(Scene& scene, float deltaTime) {

    collisionModule.add(scene.car.modelPose, modelStore.car);

    for (auto& i : scene.items) {
        if (i.type == OBSTACLE) {
            collisionModule.add(i.pose, modelStore.items[OBSTACLE]);
        } else if (i.type == DYNAMIC_OBSTACLE) {
            collisionModule.add(i.pose, modelStore.items[DYNAMIC_OBSTACLE]);
        } else if (i.type == PEDESTRIAN) {
            collisionModule.add(i.pose, modelStore.items[PEDESTRIAN]);
        } else if (i.type == DYNAMIC_PEDESTRIAN_RIGHT) {
            collisionModule.add(i.pose, modelStore.items[PEDESTRIAN]);
        } else if (i.type == DYNAMIC_PEDESTRIAN_LEFT) {
            collisionModule.add(i.pose, modelStore.items[PEDESTRIAN]);
        }
    }

    collisionModule.update();

    if (!scene.paused) {
        car.updatePosition(scene.car, deltaTime);
    }

    itemsModule.updateDynamicItems(
            deltaTime,
            scene.car, 
            scene.dynamicItemSettings,
            scene.items);

    visModule.addPositionTrace(
            scene.car.modelPose.position, 
            scene.simulationClock.time);

    car.updateMainCamera(scene.car.mainCamera, scene.car.modelPose);
    car.updateDepthCamera(scene.car.depthCamera, scene.car.modelPose);
    car.updateLaserSensors(scene.car, modelStore, scene.items);
}

void Loop::renderScene(Scene& scene, GLuint shaderProgramId) {

    light.render(shaderProgramId);

    car.render(shaderProgramId, scene.car, modelStore);

    itemsModule.render(shaderProgramId, modelStore, scene.items);

    editor.renderScene(shaderProgramId, modelStore.rect, scene.tracks, scene.groundSize);
}

void Loop::renderFpsView(Scene& scene) {

    glUseProgram(fpsShaderProgram.id);

    GLint timeLocation = glGetUniformLocation(fpsShaderProgram.id, "time");
    glUniform1f(timeLocation, (float)scene.simulationClock.time * 1000);

    GLint noiseLocation = glGetUniformLocation(fpsShaderProgram.id, "noise");
    glUniform1f(noiseLocation, 0.0f);

    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer.id);

    glViewport(0, 0, windowWidth, windowHeight);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (selectedCamera == FPS_CAMERA) {
        scene.fpsCamera.render(fpsShaderProgram.id);
    } else if (selectedCamera == FOLLOW_CAMERA) {
        scene.followCamera.render(fpsShaderProgram.id);
    }

    renderScene(scene, fpsShaderProgram.id);

    // render markers over everything else
    // thus we clear the depth buffer here

    glClear(GL_DEPTH_BUFFER_BIT);

    if (settings.showMarkers) {
        editor.renderMarkers(
                fpsShaderProgram.id,
                scene.tracks,
                scene.fpsCamera.pose.position);

        markerModule.add(light.pose, MarkerModule::TRANSLATE_ALL);
        markerModule.add(scene.car.modelPose,
                MarkerModule::TRANSLATE_X 
                | MarkerModule::TRANSLATE_Z
                | MarkerModule::ROTATE_Y);

        for (Scene::Item& i : scene.items) {
            markerModule.add(i.pose,
                    MarkerModule::TRANSLATE_X
                    | MarkerModule::TRANSLATE_Z
                    | MarkerModule::SCALE_ALL
                    | MarkerModule::ROTATE_Y);
        }

        markerModule.render(
                fpsShaderProgram.id, 
                modelStore, 
                scene.fpsCamera, 
                scene.selection);
    }

    if (settings.showVehiclePath) {
        visModule.renderPositionTrace(
                fpsShaderProgram.id,
                modelStore.marker,
                scene.simulationClock.time,
                settings.fancyVehiclePath);
    }

    visModule.renderSensors(
            fpsShaderProgram.id,
            modelStore.rect,
            modelStore.marker,
            scene.car,
            settings);

    visModule.renderDynamicItems(
            fpsShaderProgram.id,
            modelStore.arrow,
            scene.simulationClock.time,
            scene.items);

    visModule.renderVisualization(
            fpsShaderProgram.id,
            modelStore.rect,
            modelStore.marker,
            scene.visualization, settings);
}

void Loop::renderCarView(Scene& scene) {

    glUseProgram(carShaderProgram.id);

    GLint carShaderTimeLocation = 
        glGetUniformLocation(carShaderProgram.id, "time");
    glUniform1f(carShaderTimeLocation, (float)scene.simulationClock.time * 1000);

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

    renderScene(scene, carShaderProgram.id);

    // main camera image in color

    glUseProgram(fpsShaderProgram.id);

    GLint timeLocation = glGetUniformLocation(fpsShaderProgram.id, "time");
    glUniform1f(timeLocation, (float)scene.simulationClock.time * 1000);

    GLint noiseLocation = glGetUniformLocation(fpsShaderProgram.id, "noise");
    glUniform1f(noiseLocation, scene.car.mainCamera.noise);

    glBindFramebuffer(GL_FRAMEBUFFER, car.frameBuffer.id);

    glViewport(0, 0,
            scene.car.mainCamera.imageWidth,
            scene.car.mainCamera.imageHeight);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    car.mainCamera.render(carShaderProgram.id);

    renderScene(scene, carShaderProgram.id);
}

void Loop::renderDepthView(Scene& scene) {

    glUseProgram(depthCameraShaderProgram.id);

    glBindFramebuffer(GL_FRAMEBUFFER, car.depthCameraFrameBuffer.id);

    glViewport(0, 0,
            scene.car.depthCamera.depthImageWidth,
            scene.car.depthCamera.depthImageHeight);

    glClearColor(1.0f, 1.0f, 100.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    car.depthCamera.render(depthCameraShaderProgram.id);

    renderScene(scene, depthCameraShaderProgram.id);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Loop::setFramebufferSize(GLFWwindow* window, int width, int height) {

    if (this->window == window) {

        windowWidth = width;
        windowHeight = height;

        frameBuffer.resize(width, height);
    }
}
