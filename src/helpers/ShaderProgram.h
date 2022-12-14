#ifndef INC_2019_SHADERPROGRAM_H
#define INC_2019_SHADERPROGRAM_H

#include <GL/glew.h>

#include "Shader.h"

class ShaderProgram {

public:

    GLuint id;

    ShaderProgram(GLuint vertexShaderId, GLuint fragShaderId);
    ShaderProgram(Shader vertexShader, Shader fragShader);
    ShaderProgram(std::string vertexShaderPath, std::string fragShaderPath);
    ~ShaderProgram();
};

#endif
