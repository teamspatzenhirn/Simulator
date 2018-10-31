#include "Capture.h"

Capture::Capture() {

    width = 0;
    height = 0;
    pboIndex = 0;

    glGenBuffers(2, pboIds);
}

Capture::~Capture() {

    glDeleteBuffers(2, pboIds);
}

bool Capture::capture(
        GLubyte* buffer,
        GLuint width,
        GLuint height,
        GLuint channels,
        GLenum format,
        GLenum mode) {

    int dataSize = width * height * channels;

    if (this->width != width || this->height != height) {
        this->height = height;
        this->width = width;
        glBindBuffer(GL_PIXEL_PACK_BUFFER, pboIds[0]);
        glBufferData(GL_PIXEL_PACK_BUFFER, dataSize, nullptr, GL_STREAM_READ);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, pboIds[1]);
        glBufferData(GL_PIXEL_PACK_BUFFER, dataSize, nullptr, GL_STREAM_READ);
    }

    pboIndex = (pboIndex + 1) % 2;
    int nextIndex = (pboIndex + 1) % 2;

    glReadBuffer(mode);

    glBindBuffer(GL_PIXEL_PACK_BUFFER, pboIds[pboIndex]);
    glReadPixels(
        0, 0,
        width, height,
        format,
        GL_UNSIGNED_BYTE,
        nullptr);

    glBindBuffer(GL_PIXEL_PACK_BUFFER, pboIds[nextIndex]);
    GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);

    if (ptr) {
        memcpy(buffer, ptr, dataSize);
        glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
    }

    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

    return ptr != nullptr;
}
