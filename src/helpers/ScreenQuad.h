#ifndef INC_2019_SCREENQUAD_H
#define INC_2019_SCREENQUAD_H

#include <memory>
#include <iostream>
#include <functional>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "Shader.h"
#include "ShaderProgram.h"

class ScreenQuad {

    GLuint vaoId;
    GLuint vboId;

public:

    std::shared_ptr<ShaderProgram> shaderProgram;

    GLuint width;
    GLuint height;

    ScreenQuad(GLuint width, GLuint height, std::string fragmentShaderPath);
    ScreenQuad(GLuint width, GLuint height, Shader fragmentShader);
    ~ScreenQuad();

    void render(std::function<void()> renderFunction);
};

#endif
