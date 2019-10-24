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

    GLuint id = 0;

    GLsizei width = 0;
    GLsizei height = 0;
    GLsizei samples = 1;

    GLuint colorTextureId = 0;
    GLuint depthTextureId = 0;

    GLint internalFormatColor = GL_RGBA;
    GLenum formatColor = GL_RGBA;

    FrameBuffer(GLsizei width, GLsizei height);
    FrameBuffer(
            GLsizei width,
            GLsizei height, 
            GLsizei samples, 
            GLint internalFormatColor, 
            GLenum formatColor);

    ~FrameBuffer();

    void resize(GLsizei width, 
            GLsizei height, 
            GLsizei samples = -1, 
            GLint internalFormatColor = 0,
            GLenum formatColor = 0);
};

#endif
