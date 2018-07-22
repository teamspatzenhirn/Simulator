#ifndef INC_2019_CAPTURE_H
#define INC_2019_CAPTURE_H

#include <cstring>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Capture {

    const GLuint imageWidth;
    const GLuint imageHeight;
    const GLuint imageChannels;

    GLuint pboIds[2];
    int pboIndex;

public:

    Capture(GLuint width, GLuint height, GLuint channels);

    void capture(GLubyte* buffer);
};

#endif
