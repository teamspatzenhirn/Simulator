#ifndef INC_2019_SHADER_H
#define INC_2019_SHADER_H

#include <string>

#include <GL/glew.h>

class Shader {
    
public:

    GLuint id;
    GLenum type;

    Shader(std::string sourcePath, GLenum shaderType);
    ~Shader();
};

#endif
