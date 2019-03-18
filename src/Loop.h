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
#include "modules/VisModule.h"

class Loop {

    GLFWwindow* window;

    GLsizei windowWidth;
    GLsizei windowHeight;

    Settings settings;

    enum SelectedCamera {
        FPS_CAMERA,
        MAIN_CAMERA,
        DEPTH_CAMERA
    } selectedCamera = FPS_CAMERA;

    ShaderProgram shaderProgram{
        Shader("shaders/VertexShader.glsl", GL_VERTEX_SHADER),
        Shader("shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER)};

    ShaderProgram carShaderProgram{
        Shader("shaders/BayerVertexShader.glsl", GL_VERTEX_SHADER),
        Shader("shaders/BayerFragmentShader.glsl", GL_FRAGMENT_SHADER)};

    ShaderProgram depthCameraShaderProgram{
        Shader("shaders/BayerVertexShader.glsl", GL_VERTEX_SHADER),
        Shader("shaders/DepthPointsFragmentShader.glsl", GL_FRAGMENT_SHADER)};

    ModelStore modelStore{"./"};

    FrameBuffer frameBuffer;
    ScreenQuad screenQuad;

    CommModule commModule;
    MarkerModule markerModule;
    GuiModule guiModule;
    ItemsModule itemsModule;
    CollisionModule collisionModule;
    RuleModule ruleModule;
    VisModule visModule;
    CarModule car;
    Editor editor;

    PointLight light{10.0f, 10.0f, 20.0f};

    void update(Scene& scene, float deltaTime);

    void renderScene(Scene& scene, GLuint shaderProgramId);
    void renderFpsView(Scene& scene);
    void renderCarView(Scene& scene);
    void renderDepthView(Scene& scene);

public:

    Loop(GLFWwindow* window, GLsizei windowWidth, GLsizei windowHeight, Settings settings);

    void loop(Scene& scene, Settings& settings);
    void step(Scene& scene, Settings& settings, float frameDeltaTime);

    void setFramebufferSize(GLFWwindow* window, int width, int height);
};

#endif
