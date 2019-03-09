#include "FrameBuffer.h"

FrameBuffer::FrameBuffer(GLsizei width, GLsizei height) 
    : FrameBuffer(width, height, GL_RGBA, GL_RGBA) {
}

FrameBuffer::FrameBuffer(GLsizei width, GLsizei height, GLint internalFormatColor, GLenum formatColor) {

    this->internalFormatColor = internalFormatColor;
    this->formatColor = formatColor;

    glGenFramebuffers(1, &id);
    glBindFramebuffer(GL_FRAMEBUFFER, id);

    // color texture

    glGenTextures(1, &colorTextureId);
    glBindTexture(GL_TEXTURE_2D, colorTextureId);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 internalFormatColor,
                 width,
                 height,
                 0,
                 formatColor,
                 GL_UNSIGNED_BYTE,
                 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glFramebufferTexture(
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorTextureId, 0);

    // depth texture

    glGenTextures(1, &depthTextureId);
    glBindTexture(GL_TEXTURE_2D, depthTextureId);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_DEPTH_COMPONENT,
                 width,
                 height,
                 0,
                 GL_DEPTH_COMPONENT,
                 GL_FLOAT,
                 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glFramebufferTexture(
            GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTextureId, 0);

    GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBuffers);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "A framebuffer was not completly initialized!" << std::endl;
        std::exit(-1);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    this->width = width;
    this->height = height;
}

FrameBuffer::~FrameBuffer() {

    glDeleteFramebuffers(1, &id);
    glDeleteTextures(1, &colorTextureId);
    glDeleteTextures(1, &depthTextureId);
}

void FrameBuffer::resize(GLsizei newWidth, GLsizei newHeight) {

    glBindTexture(GL_TEXTURE_2D, colorTextureId);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 internalFormatColor,
                 newWidth,
                 newHeight,
                 0,
                 formatColor,
                 GL_UNSIGNED_BYTE,
                 0);

    glBindTexture(GL_TEXTURE_2D, depthTextureId);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_DEPTH_COMPONENT24,
                 newWidth,
                 newHeight,
                 0,
                 GL_DEPTH_COMPONENT,
                 GL_FLOAT,
                 0);

    glBindTexture(GL_TEXTURE_2D, 0);

    this->width = newWidth;
    this->height = newHeight;
}

