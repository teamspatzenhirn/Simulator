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
 * rgba texture for the color and 24 bit depth texture for the
 * depth information.
 */
class FrameBuffer {

public:

    GLuint id;

    GLuint colorTextureId;
    GLuint depthTextureId;

    FrameBuffer(GLuint width, GLuint height);
    ~FrameBuffer();
};

#endif
