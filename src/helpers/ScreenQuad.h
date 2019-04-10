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

    ScreenQuad(
            std::string vertexShaderPath, 
            std::string fragmentShaderPath);

    ScreenQuad(
            Shader vertexShader, 
            Shader fragmentShader);

    ~ScreenQuad();

    /*
     * Starts drawing to screen filling quad.
     * Returns the shaderProgramId for this screen quad
     * and binds the shaderProgram.
     */
    GLuint start();

    /*
     * Ends drawing to the screen filling quad.
     * Actually draws the geometry of the screen quad.
     */
    void end();

    void render(std::function<void(GLuint)> renderFunction);
};

#endif
