#ifndef INC_2019_LOOP_H
#define INC_2019_LOOP_H

#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>

#include "helpers/Helpers.h" 
#include "modules/MarkerModule.h"
#include "modules/CarModule.h"
#include "modules/GuiModule.h"
#include "modules/Editor.h"
#include "sharedmem/shmcomm.h"

class Loop {

    static constexpr int sharedMemId = 428769;

public:

    static std::shared_ptr<Loop> instance;

private:

    GLFWwindow* window;

    GLuint windowWidth;
    GLuint windowHeight;

    bool fpsCameraActive{true};

    Timer timer;

    ShaderProgram shaderProgram{
        Shader("shaders/VertexShader.glsl", GL_VERTEX_SHADER),
        Shader("shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER)};

    Scene scene;

    FrameBuffer frameBuffer;
    FrameBuffer markerFrameBuffer;

    ScreenQuad screenQuad;
    ScreenQuad screenQuadCar;

    MarkerModule markerModule;
    GuiModule guiModule;

    PointLight light{10.0f, 10.0f, 20.0f};
    Pose modelPose;
    Model cube{"models/arrow.obj"};

    FpsCamera fpsCamera{M_PI * 0.3f, 4.0f/3.0f};

    Editor editor{scene.groundSize};

    CarModule car;

    Capture capture;

    struct ImageObject {
        // assuming 2064x1544 RGB as the maximum possible image size
        unsigned char buffer[9560448];
        int imageWidth;
        int imageHeight;
    };

    SimulatorSHM::SHMComm<ImageObject> tx; 

public:

    Loop(GLFWwindow* window, GLuint windowWidth, GLuint windowHeight);

    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

    void loop();

    void update(double deltaTime);

    void renderScene();
    void renderFpsView();
    void renderCarView();
};

#endif
