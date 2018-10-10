#ifndef INC_2019_LOOP_H
#define INC_2019_LOOP_H

#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>

#include "helpers/Helpers.h" 
#include "modules/MarkerModule.h"
#include "modules/CarModule.h"
#include "modules/CommModule.h"
#include "modules/GuiModule.h"
#include "modules/Editor.h"
#include "modules/ItemsModule.h"

class Loop {

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

    CommModule commModule;
    MarkerModule markerModule;
    GuiModule guiModule;
    ItemsModule itemsModule;

    PointLight light{10.0f, 10.0f, 20.0f};

    FpsCamera fpsCamera{M_PI * 0.3f, 4.0f/3.0f};

    Editor editor{scene.groundSize};

    CarModule car;

public:

    static std::shared_ptr<Loop> instance;

    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

    Loop(GLFWwindow* window, GLuint windowWidth, GLuint windowHeight);

    void loop();

    void update(double deltaTime);

    void renderScene();
    void renderFpsView();
    void renderCarView();
};

#endif
