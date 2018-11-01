#ifndef INC_2019_LOOP_H
#define INC_2019_LOOP_H

#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <deque>

#include "helpers/Helpers.h" 
#include "modules/MarkerModule.h"
#include "modules/CarModule.h"
#include "modules/CommModule.h"
#include "modules/GuiModule.h"
#include "modules/Editor.h"
#include "modules/ItemsModule.h"
#include "modules/CollisionModule.h"
#include "modules/RuleModule.h"

class Loop {

    GLFWwindow* window;

    GLuint windowWidth;
    GLuint windowHeight;

    bool fpsCameraActive{true};

    Timer timer;

    ShaderProgram shaderProgram{
        Shader("shaders/VertexShader.glsl", GL_VERTEX_SHADER),
        Shader("shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER)};

    ShaderProgram carShaderProgram{
        Shader("shaders/BayerVertexShader.glsl", GL_VERTEX_SHADER),
        Shader("shaders/BayerFragmentShader.glsl", GL_FRAGMENT_SHADER)};

    Scene scene;

    FrameBuffer frameBuffer;
    FrameBuffer markerFrameBuffer;

    ScreenQuad screenQuad;
    ScreenQuad screenQuadCar;

    CommModule commModule;
    MarkerModule markerModule;
    GuiModule guiModule;
    ItemsModule itemsModule;
    CollisionModule collisionModule;
    RuleModule ruleModule;

    PointLight light{10.0f, 10.0f, 20.0f};

    Editor editor{scene.groundSize};

    CarModule car;

    void update(double deltaTime);
    void updateCollisions();

    void renderMarkers(GLuint shaderProgramId);
    void renderScene(GLuint shaderProgramIdj);
    void renderFpsView();
    void renderCarView();

public:

    static std::shared_ptr<Loop> instance;

    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

    Loop(GLFWwindow* window, GLuint windowWidth, GLuint windowHeight, std::string scenePath);

    void loop();
};

#endif
