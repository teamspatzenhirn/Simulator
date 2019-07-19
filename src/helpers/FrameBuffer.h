#ifndef INC_2019_FRAMEBUFFER_H
#define INC_2019_FRAMEBUFFER_H

#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

/*
 * This is a very thin wrapper around the framebuffer concept.
 * It simply ensures that the framebuffer and the textures are
 * deleted whenever the destructor of the object is called.
 *
 * Internally a frame buffer object is created that holds a
 * rgba texture for the color and 24 bit depth texture for 
 * depth information.
 */
class FrameBuffer {

public:

    GLuint id;

    GLsizei width;
    GLsizei height;
    GLsizei samples;

    GLuint colorTextureId;
    GLuint depthTextureId;

    GLint internalFormatColor;
    GLenum formatColor;

    FrameBuffer(GLsizei width, GLsizei height);
    FrameBuffer(
            GLsizei width,
            GLsizei height, 
            GLsizei samples, 
            GLint internalFormatColor, 
            GLenum formatColor);

    ~FrameBuffer();

    void resize(GLsizei newWidth, GLsizei newHeight, GLsizei newSamples = -1);
};

#endif
