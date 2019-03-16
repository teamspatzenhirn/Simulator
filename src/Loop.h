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

    Timer timer;

    ShaderProgram shaderProgram{
        Shader("shaders/VertexShader.glsl", GL_VERTEX_SHADER),
        Shader("shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER)};

    ShaderProgram carShaderProgram{
        Shader("shaders/BayerVertexShader.glsl", GL_VERTEX_SHADER),
        Shader("shaders/BayerFragmentShader.glsl", GL_FRAGMENT_SHADER)};

    ShaderProgram depthCameraShaderProgram{
        Shader("shaders/BayerVertexShader.glsl", GL_VERTEX_SHADER),
        Shader("shaders/DepthPointsFragmentShader.glsl", GL_FRAGMENT_SHADER)};

    ModelStore modelStore;

    Scene scene;

    FrameBuffer frameBuffer;
    ScreenQuad screenQuad;

    CommModule commModule;
    MarkerModule markerModule;
    GuiModule guiModule;
    ItemsModule itemsModule;
    CollisionModule collisionModule;
    RuleModule ruleModule;
    VisModule visModule;

    PointLight light{10.0f, 10.0f, 20.0f};

    Editor editor{scene.tracks, scene.groundSize};

    CarModule car;

    void update(Scene& scene, float deltaTime, float simDeltaTime);

    void renderScene(GLuint shaderProgramIdj);
    void renderFpsView(Scene& scene);
    void renderCarView(Scene& scene);
    void renderDepthView(Scene& scene);

public:

    static std::shared_ptr<Loop> instance;

    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

    Loop(GLFWwindow* window, GLsizei windowWidth, GLsizei windowHeight, Settings settings);

    void loop();
};

#endif
