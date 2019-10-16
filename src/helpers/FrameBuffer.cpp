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

    glGenFramebuffers(1, &id);

    resize(width, height, samples, internalFormatColor, formatColor);

    glBindFramebuffer(GL_FRAMEBUFFER, id);

    GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBuffers);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "A framebuffer was not completly initialized!" << std::endl;
        std::exit(-1);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FrameBuffer::~FrameBuffer() {

    glDeleteFramebuffers(1, &id);
    glDeleteTextures(1, &colorTextureId);
    glDeleteTextures(1, &depthTextureId);
}

void FrameBuffer::resize(
        GLsizei width, 
        GLsizei height, 
        GLsizei samples, 
        GLint internalFormatColor,
        GLenum formatColor) {

    if (samples < 1) {
        samples = this->samples;
    }

    // only resize if absolutely necessary

    if (this->width == width
            && this->height == height
            && this->samples == samples
            && this->internalFormatColor == internalFormatColor
            && this->formatColor == formatColor) { 
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, id);

    // color texture

    if (samples != this->samples
            || internalFormatColor != this->internalFormatColor
            || formatColor != this-> formatColor
            || colorTextureId == 0) { 
        if (colorTextureId != 0) {
            glDeleteTextures(1, &colorTextureId);
        } 
        glGenTextures(1, &colorTextureId);
    }

    if (samples > 1) {
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
    } else {
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
    }

    // depth texture

    if (samples != this->samples || depthTextureId == 0) { 
        if (depthTextureId != 0) {
            glDeleteTextures(1, &depthTextureId);
        }
        glGenTextures(1, &depthTextureId);
    }

    if (samples > 1) {
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
    } else {
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
    }

    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    this->width = width;
    this->height = height;
    this->samples = samples;
    this->internalFormatColor = internalFormatColor;
    this->formatColor = formatColor;
}

