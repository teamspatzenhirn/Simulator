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
    , guiModule{window}
    , tx(SimulatorSHM::SERVER, 428769) {

    fpsCamera.view = glm::translate(fpsCamera.view, glm::vec3(0.0f, 0.0f, -4.0f));

    cube.upload();

    if(!tx.attach()) {
        tx.destroy();
        if(!tx.attach()) {
            std::cout << "Shared memory init failed!" << std::endl;
            std::exit(-1);
        }
    }
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

        markerModule.add(modelPose);
        
        glUseProgram(shaderProgram.id);

        // render fps / editor camera view if needed

        for(KeyEvent& e : getKeyEvents()) {
            if (e.key == GLFW_KEY_C && e.action == GLFW_PRESS) {
                fpsCameraActive = !fpsCameraActive;
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

        guiModule.end(scene);

        glfwSwapBuffers(window);
    }
}

void Loop::update(double deltaTime) {

    /* modelPose.rotation = glm::rotate(
           modelPose.rotation, 0.002f, glm::vec3(0, 0, 1)); */

    fpsCamera.update(window, deltaTime);

    car.update(scene.car, deltaTime);
}

void Loop::renderScene() {

    light.render(shaderProgram.id);

    cube.render(shaderProgram.id, modelPose.getMatrix());

    car.render(scene.car, shaderProgram.id, markerModule);

    editor.renderScene(shaderProgram.id, scene.tracks);
}

void Loop::renderFpsView() {

    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer.id);

    glViewport(0, 0, windowWidth, windowHeight);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    fpsCamera.render(shaderProgram.id);

    renderScene();

    // render module guis

    car.renderCarPropertiesGui(scene.car, guiModule);

    // render marker overlay 

    glBindFramebuffer(GL_FRAMEBUFFER, markerFrameBuffer.id);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    markerModule.render(window, shaderProgram.id, fpsCamera, guiModule);

    editor.updateInput(fpsCamera, scene.tracks, scene.groundSize);
    editor.renderMarkers(shaderProgram.id, scene.tracks);
}

void Loop::renderCarView() {

    glBindFramebuffer(GL_FRAMEBUFFER, car.frameBuffer.id);

    glViewport(0, 0,
            scene.car.mainCamera.imageWidth,
            scene.car.mainCamera.imageHeight);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    car.mainCamera.render(shaderProgram.id);

    renderScene();

    // download image from opengl to shared memory buffer

    ImageObject* obj = tx.lock(SimulatorSHM::WRITE_NO_OVERWRITE); 

    if (obj != NULL) {
        capture.capture(obj->buffer,
                scene.car.mainCamera.imageWidth,
                scene.car.mainCamera.imageHeight,
                GL_COLOR_ATTACHMENT0);
        obj->imageWidth = scene.car.mainCamera.imageWidth;
        obj->imageHeight = scene.car.mainCamera.imageHeight;
        tx.unlock(obj);
    } else {
        //std::cout << "ADTF down!" << std::endl;
    }
}
