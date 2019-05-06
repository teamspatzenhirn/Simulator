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

    FrameBuffer frameBuffer;

    Settings settings;

    ScreenQuad screenQuad;

    ShaderProgram fpsShaderProgram;
    ShaderProgram carShaderProgram;
    ShaderProgram depthCameraShaderProgram;

    ModelStore modelStore{settings.resourcePath};

    enum SelectedCamera {
        FPS_CAMERA,
        FOLLOW_CAMERA,
        MAIN_CAMERA,
        DEPTH_CAMERA,
    } selectedCamera = FPS_CAMERA;

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

    void loop(Scene& scene);
    void step(Scene& scene, float frameDeltaTime);

    void setFramebufferSize(GLFWwindow* window, int width, int height);
};

#endif
