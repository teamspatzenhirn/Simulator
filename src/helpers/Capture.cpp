#include "Capture.h"

#include <cstring>

Capture::Capture() {

    pboWidth = 0;
    pboHeight = 0;
    pboIndex = 0;

    glGenBuffers(2, pboIds);
}

Capture::~Capture() {

    glDeleteBuffers(2, pboIds);
}

bool Capture::capture(
        GLubyte* buffer,
        GLsizei width,
        GLsizei height,
        GLsizei elementSize,
        GLenum format,
        GLenum dataType) {

    GLsizei dataSize = width * height * elementSize;

    if (pboWidth != width || pboHeight != height) {
        pboHeight = height;
        pboWidth = width;
        glBindBuffer(GL_PIXEL_PACK_BUFFER, pboIds[0]);
        glBufferData(GL_PIXEL_PACK_BUFFER, dataSize, nullptr, GL_STREAM_READ);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, pboIds[1]);
        glBufferData(GL_PIXEL_PACK_BUFFER, dataSize, nullptr, GL_STREAM_READ);
    }

    pboIndex = (pboIndex + 1) % 2;
    int nextIndex = (pboIndex + 1) % 2;

    glBindBuffer(GL_PIXEL_PACK_BUFFER, pboIds[pboIndex]);
    glReadPixels(
        0, 0,
        width, height,
        format,
        dataType,
        nullptr);

    glBindBuffer(GL_PIXEL_PACK_BUFFER, pboIds[nextIndex]);
    GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);

    if (ptr) {
        memcpy(buffer, ptr, (size_t)dataSize);
        glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
    }

    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

    return ptr != nullptr;
}
