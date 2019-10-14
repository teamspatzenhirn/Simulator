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

public:

    GLFWwindow* window;

    Settings settings;

    FrameBuffer screenFrameBuffer;
    FrameBuffer frameBuffer;
    ScreenQuad screenQuad;

    ShaderProgram fpsShaderProgram;
    ShaderProgram carShaderProgram;
    ShaderProgram depthCameraShaderProgram;

    Capture pythonMainCameraCapture;
    Capture mainCameraCapture;
    Capture depthCameraCapture;

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

    Loop(Settings settings);
    ~Loop();

    void update(Scene& scene, float deltaTime);

    void renderScene(Scene& scene, GLuint shaderProgramId);
    void renderFpsView(Scene& scene);
    void renderCarView(Scene& scene);
    void renderDepthView(Scene& scene);

    void loop(Scene& scene);
    void step(Scene& scene, float frameDeltaTime);
};

#endif
