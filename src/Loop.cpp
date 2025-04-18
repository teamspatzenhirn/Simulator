#include "Loop.h"

GLFWwindow* setupGlfw(Settings& settings) {

    if (!glfwInit()) {
        std::cout << "Could not initialize GLFW!" << std::endl;
        std::exit(-1);
    }

    glfwWindowHint(GLFW_SAMPLES, settings.msaaSamplesEditorView);

    GLFWwindow* window = glfwCreateWindow(
            settings.windowWidth,
            settings.windowHeight,
            "SpatzSim",
            nullptr,
            nullptr);

    if (settings.fullscreen) {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    }

    glfwMakeContextCurrent(window);

    glewExperimental = true;

    if (GLEW_OK != glewInit()) {
        std::cout << "GL Extension Wrangler initialization failed!" << std::endl;
        std::exit(-1);
    }

    return window;
}

Loop::Loop(Settings settings)
    : window{setupGlfw(settings)}
    , settings{settings}
    , screenFrameBuffer{settings.windowWidth, settings.windowHeight}
    , frameBuffer{
        settings.windowWidth,
        settings.windowHeight,
        settings.msaaSamplesEditorView,
        GL_RGBA,
        GL_RGBA}
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

    glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glfwSwapInterval(0);

    initInput(window);
}

Loop::~Loop() {

    glfwDestroyWindow(window);
    glfwTerminate();
}

void renderToScreen (
        GLsizei windowWidth, 
        GLsizei windowHeight, 
        ScreenQuad& screenQuad, 
        float aspectRatio,
        bool keepAspectRatio,
        FrameBuffer& srcFrameBuffer,
        FrameBuffer& dstFrameBuffer) {

    dstFrameBuffer.resize(
            srcFrameBuffer.width, srcFrameBuffer.height);

    // breaks down multisampled framebuffer to single sample texture

    glBindFramebuffer(GL_READ_FRAMEBUFFER, srcFrameBuffer.id);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dstFrameBuffer.id);

    glBlitFramebuffer(
            0, 0, srcFrameBuffer.width, srcFrameBuffer.height, 
            0, 0, srcFrameBuffer.width, srcFrameBuffer.height, 
            GL_COLOR_BUFFER_BIT, GL_NEAREST);

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

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    GLuint shaderProgramId = screenQuad.start();

    glUseProgram(shaderProgramId);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, dstFrameBuffer.colorTextureId);
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

        step(scene, frameDeltaTime);
    }
}

void Loop::step(Scene& scene, float frameDeltaTime) {

    scene.displayClock.windup(frameDeltaTime); 
    if (!scene.paused) {
        scene.simulationClock.windup(frameDeltaTime * settings.simulationSpeed); 
    }

    updateInput();

    // TODO: make this less hacky, this is not right here

    for (MouseButtonEvent& evt : getMouseButtonEvents()) {
        if (evt.action == GLFW_PRESS && evt.button == GLFW_MOUSE_BUTTON_LEFT) {
            scene.selection.handled = false;
        }
    }

    guiModule.begin();

    // gui updates 

    while (scene.displayClock.step(settings.updateDeltaTime)) {

        commModule.receiveVisualization(scene.visualization);

        if (FPS_CAMERA == selectedCamera) {
            scene.fpsCamera.update(window, settings.updateDeltaTime);
        } else if (FOLLOW_CAMERA == selectedCamera) {
            scene.followCamera.update(scene.car.modelPose);
        } else if (CINEMATIC_CAMERA == selectedCamera) {
            scene.cinematicCamera.update(scene.car.modelPose);
        } else if (ORTHO_CAMERA == selectedCamera) {
            scene.orthoCamera.update(window, settings.updateDeltaTime);
        }
    }

    for(KeyEvent& e : getKeyEvents()) {
        if (e.key == GLFW_KEY_C && e.action == GLFW_PRESS) {
            selectedCamera = (SelectedCamera) ((((int) selectedCamera) + 1) % 5);
            if(selectedCamera == CINEMATIC_CAMERA){
                scene.cinematicCamera.pose = scene.fpsCamera.pose;
            }
        }
        if (e.key == GLFW_KEY_P && e.action == GLFW_PRESS) {
            scene.paused = !scene.paused;
        }
    }

    float aspectRatio = (float)settings.windowWidth / (float)settings.windowHeight;
    switch(selectedCamera) {
        case FPS_CAMERA:
            scene.fpsCamera.aspectRatio = aspectRatio;
            break;
        case FOLLOW_CAMERA:
            scene.followCamera.aspectRatio = aspectRatio;
            break;
        case CINEMATIC_CAMERA:
            scene.cinematicCamera.aspectRatio = aspectRatio;
            break;
        case ORTHO_CAMERA:
            scene.orthoCamera.aspectRatio = aspectRatio;
            break;
        default:
            break;
    }

    if (FPS_CAMERA == selectedCamera) {
        if (settings.showMarkers) {
            markerModule.update(window, scene.fpsCamera, scene.selection);
            editor.updateInput(scene.fpsCamera, scene.tracks, scene.groundSize);
        }

        itemsModule.update(scene.items, scene.selection.pose);
    }

    // actual simulation updates
    
    while (scene.simulationClock.step(settings.updateDeltaTime)) {

        if (scene.enableAutoTracks) {
            autoTracks.update(scene);
        }

        // TODO: Doing receive in such a way is not really correct!
        // Likely the vesc value will not actually change n-times
        // during the iteration. Probably, we will read the same
        // value n-times. We need to make sure that the buffer
        // queue in the shared memory is actually used.

        commModule.receiveVesc(scene.car.vesc);

        if (scene.failTime == 0 || !settings.instantCloseInAutotrack) {
            update(scene, settings.updateDeltaTime);
        } else if (scene.displayClock.time - scene.failTime > 5.0) {
            exit(-1);
        }

        bool noViolation = ruleModule.update(
                scene.displayClock.time,
                scene.simulationClock.time,
                scene.rules,
                scene.car,
                scene.tracks,
                scene.items,
                collisionModule);

        if (scene.failTime != 0 && scene.enableAutoTracks && noViolation) {
            scene.failTime = 0;
        }

        if (scene.failTime == 0 && scene.enableAutoTracks && !noViolation) {
            scene.failTime = scene.displayClock.time;

            ruleModule.printViolation(
                    scene.simulationClock.time,
                    scene.car.drivenDistance);
        }

        commModule.transmitCar(
                scene.car, 
                scene.paused, 
                scene.simulationClock.time);
    }

    // start rendering camera images

    scene.addToHistory();

    // copy scene then update by the amount of time in the accumulator.
    // this will give a very smooth rendering result even though the
    // actual values in the scene are likely not correct due to floating
    // point errors and because the accumulator might be small.
    // this is why the scene is copied here and restored at mark (1)
    
    Scene preRenderScene = scene;


    if (scene.failTime == 0) {
        update(scene, scene.simulationClock.accumulator);
    }

    if (FPS_CAMERA == selectedCamera) {
        scene.fpsCamera.update(window, scene.displayClock.accumulator);
    } else if (FOLLOW_CAMERA == selectedCamera) {
        scene.followCamera.update(scene.car.modelPose);
    } else if (CINEMATIC_CAMERA == selectedCamera) {
        scene.cinematicCamera.update(scene.car.modelPose);
    }

    renderCarView(scene);
    renderDepthView(scene);
    renderFpsView(scene);

    // render on screen filling quad

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    if (MAIN_CAMERA == selectedCamera) {
        renderToScreen(
                settings.windowWidth, 
                settings.windowHeight, 
                screenQuad, 
                scene.car.mainCamera.getAspectRatio(), 
                true, 
                car.frameBuffer,
                screenFrameBuffer);
    } else if (DEPTH_CAMERA == selectedCamera) {
        renderToScreen(
                settings.windowWidth, 
                settings.windowHeight, 
                screenQuad, 
                scene.car.depthCamera.getDepthAspectRatio(), 
                true, 
                car.depthCameraFrameBuffer,
                screenFrameBuffer);
    } else { // FPS_CAMERA or FOLLOW_CAMERA or CINEMATIC_CAMERA
        renderToScreen(
                settings.windowWidth, 
                settings.windowHeight, 
                screenQuad, 
                1, 
                false, 
                frameBuffer,
                screenFrameBuffer);
    }

    // mark (1): restore scene saved before the rendering

    scene = preRenderScene;

    if (guiModule.renderSettingsWindow(settings)) {
        glfwSetWindowSize(
                window,
                settings.windowWidth,
                settings.windowHeight);

        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        if (settings.fullscreen) {
            glfwSetWindowMonitor(window, monitor, 0, 0,
                    mode->width, mode->height, mode->refreshRate);
        } else {
            glfwSetWindowMonitor(window, nullptr, 0, 0,
                    settings.windowWidth, settings.windowHeight, mode->refreshRate);
        }
    } else {
        glfwGetWindowSize(
                window,
                &settings.windowWidth,
                &settings.windowHeight);
    }

    guiModule.renderRootWindow(scene, settings);
    guiModule.renderSceneWindow(scene);
    guiModule.renderRuleWindow(scene.rules);
    guiModule.renderHelpWindow();
    guiModule.renderAboutWindow();

    guiModule.end();

    commModule.transmitMainCamera(
            scene.car, 
            mainCameraCapture, 
            car.bayerFrameBuffer.id);

    commModule.transmitDepthCamera(
            scene.car, 
            depthCameraCapture, 
            car.depthCameraFrameBuffer.id);

    glfwSwapBuffers(window);
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

    visModule.addPoseTrace(
            scene.car.modelPose,
            scene.simulationClock.time);

    car.updateMainCamera(scene.car.mainCamera, scene.car.modelPose);
    car.updateDepthCamera(scene.car.depthCamera, scene.car.modelPose);
    car.updateLaserSensors(scene.car, modelStore, scene.items);
}

void Loop::renderScene(Scene& scene, GLuint shaderProgramId) {

    scene.light.render(shaderProgramId);

    car.render(shaderProgramId, scene.car, modelStore);

    itemsModule.render(shaderProgramId, modelStore, scene.items);

    editor.renderScene(shaderProgramId, modelStore.rect, scene.tracks, scene.groundSize);
}

void Loop::renderFpsView(Scene& scene) {

    // make sure that framebuffer is resize properly
    
    frameBuffer.resize(
            settings.windowWidth,
            settings.windowHeight,
            settings.msaaSamplesEditorView);

    // render the actual scene first

    glUseProgram(fpsShaderProgram.id);

    GLint timeLocation = glGetUniformLocation(fpsShaderProgram.id, "time");
    glUniform1f(timeLocation, (float)scene.simulationClock.time * 1000);

    GLint noiseLocation = glGetUniformLocation(fpsShaderProgram.id, "noise");
    glUniform1f(noiseLocation, 0.0f);

    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer.id);

    glViewport(0, 0, settings.windowWidth, settings.windowHeight);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (selectedCamera == FPS_CAMERA) {
        scene.fpsCamera.render(fpsShaderProgram.id);
    } else if (selectedCamera == FOLLOW_CAMERA) {
        scene.followCamera.render(fpsShaderProgram.id);
    } else if (selectedCamera == CINEMATIC_CAMERA) {
        scene.cinematicCamera.render(fpsShaderProgram.id);
    } else if (selectedCamera == ORTHO_CAMERA) {
        scene.orthoCamera.render(fpsShaderProgram.id);
    }

    renderScene(scene, fpsShaderProgram.id);

    // render markers over everything else
    // thus we clear the depth buffer here

    glClear(GL_DEPTH_BUFFER_BIT);

    if (selectedCamera == FPS_CAMERA && settings.showMarkers) {
        editor.renderMarkers(
                fpsShaderProgram.id,
                scene.tracks,
                scene.fpsCamera.pose.position);

        markerModule.add(scene.light.pose, MarkerModule::TRANSLATE_ALL);
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
        visModule.renderPoseTrace(
                fpsShaderProgram.id,
                modelStore.marker,
                scene.simulationClock.time,
                settings.fancyVehiclePath);
    }

    /*
     * Enable for debugging ...
     * TODO: option in settings
    visModule.renderTrackPath(
            fpsShaderProgram.id, 
            modelStore.marker, 
            scene.tracks);
    */

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

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    car.depthCamera.render(depthCameraShaderProgram.id);

    renderScene(scene, depthCameraShaderProgram.id);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
