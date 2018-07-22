#ifndef INC_2019_SHADER_H
#define INC_2019_SHADER_H

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Shader {
    
public:

    GLuint id;
    GLenum type;

    Shader(std::string sourcePath, GLenum shaderType);
    ~Shader();
};

#endif
