#ifndef INC_2019_CAPTURE_H
#define INC_2019_CAPTURE_H

#include <cstring>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Capture {

    GLuint pboIds[2];
    int pboIndex;

    GLuint width;
    GLuint height;

public:

    Capture();
    ~Capture();

    bool capture(
            GLubyte* buffer,
            GLuint width,
            GLuint height,
            GLuint elementSize,
            GLenum format,
            GLenum dataType = GL_UNSIGNED_BYTE);
};

#endif
