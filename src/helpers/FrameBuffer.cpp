#include "FrameBuffer.h"

FrameBuffer::FrameBuffer(GLsizei width, GLsizei height) 
    : FrameBuffer(width, height, 1, GL_RGBA, GL_RGBA) {
}

FrameBuffer::FrameBuffer(
        GLsizei width, 
        GLsizei height, 
        GLsizei samples, 
        GLint internalFormatColor, 
        GLenum formatColor) {

    this->internalFormatColor = internalFormatColor;
    this->formatColor = formatColor;

    glGenFramebuffers(1, &id);
    glBindFramebuffer(GL_FRAMEBUFFER, id);

    // color texture

    glGenTextures(1, &colorTextureId);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, colorTextureId);

    glTexImage2DMultisample(
                 GL_TEXTURE_2D_MULTISAMPLE,
                 samples,
                 internalFormatColor,
                 width,
                 height,
                 false);

    glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D_MULTISAMPLE,
            colorTextureId,
            0);

    // depth texture

    glGenTextures(1, &depthTextureId);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, depthTextureId);

    glTexImage2DMultisample(
                 GL_TEXTURE_2D_MULTISAMPLE,
                 samples,
                 GL_DEPTH_COMPONENT,
                 width,
                 height,
                 false);

    glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_DEPTH_ATTACHMENT,
            GL_TEXTURE_2D_MULTISAMPLE,
            depthTextureId,
            0);

    GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBuffers);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "A framebuffer was not completly initialized!" << std::endl;
        std::exit(-1);
    }

    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    this->width = width;
    this->height = height;
}

FrameBuffer::~FrameBuffer() {

    glDeleteFramebuffers(1, &id);
    glDeleteTextures(1, &colorTextureId);
    glDeleteTextures(1, &depthTextureId);
}

void FrameBuffer::resize(GLsizei newWidth, GLsizei newHeight, GLsizei samples) {

    width = newWidth;
    height = newHeight;

    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, colorTextureId);

    glTexImage2DMultisample(
                 GL_TEXTURE_2D_MULTISAMPLE,
                 samples,
                 internalFormatColor,
                 width,
                 height,
                 false);

    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, depthTextureId);

    glTexImage2DMultisample(
                 GL_TEXTURE_2D_MULTISAMPLE,
                 samples,
                 GL_DEPTH_COMPONENT,
                 width,
                 height,
                 false);

    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
}

