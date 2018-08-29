#ifndef INC_2019_LOOP_H
#define INC_2019_LOOP_H

#define _USE_MATH_DEFINES
#include <cmath>

#include "helpers/Helpers.h" 
#include "modules/MarkerModule.h"
#include "modules/CarModule.h"
#include "modules/GuiModule.h"

class Loop {

    static constexpr int carCameraWidth = 2064;
    static constexpr int carCameraHeight = 1544;
    static constexpr float carCameraAspect = ((float) carCameraWidth) / carCameraHeight;

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

    FrameBuffer frameBuffer;
    FrameBuffer markerFrameBuffer;

    ScreenQuad screenQuad;
    ScreenQuad screenQuadCar;

    MarkerModule markerModule;
    GuiModule guiModule;

    FpsCamera camera{M_PI * 0.3f, 4.0f/3.0f};

    PointLight light{10.0f, 10.0f, 20.0f};

    Pose modelPose;

    CarModule car{glm::vec3(0.0f, 0.0f, 2.0f), M_PI * 0.5f, carCameraAspect};

    Model cube{"models/test_cube.obj"};

public:

    Loop(GLFWwindow* window, GLuint windowWidth, GLuint windowHeight);

    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    void loop();
};

#endif
