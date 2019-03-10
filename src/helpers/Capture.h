#ifndef INC_2019_CAPTURE_H
#define INC_2019_CAPTURE_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Capture {

    GLsizei pboWidth;
    GLsizei pboHeight;

    int pboIndex;
    GLuint pboIds[2];

public:

    Capture();
    ~Capture();

    bool capture(
            GLubyte* buffer,
            GLsizei width,
            GLsizei height,
            GLsizei elementSize,
            GLenum format,
            GLenum dataType = GL_UNSIGNED_BYTE);
};

#endif
