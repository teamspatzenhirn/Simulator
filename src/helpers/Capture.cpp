#include "Capture.h"

Capture::Capture(GLuint width, GLuint height, GLuint channels = 3)
        : imageWidth(width),
          imageHeight(height),
          imageChannels(channels) {

    pboIndex = 0;

    int dataSize = width * height * channels;

    glGenBuffers(2, pboIds);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, pboIds[0]);
    glBufferData(GL_PIXEL_PACK_BUFFER, dataSize, nullptr, GL_STREAM_READ);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, pboIds[1]);
    glBufferData(GL_PIXEL_PACK_BUFFER, dataSize, nullptr, GL_STREAM_READ);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}

void Capture::capture(GLubyte* buffer) {

    pboIndex = (pboIndex + 1) % 2;
    int nextIndex = (pboIndex + 1) % 2;

    glReadBuffer(GL_FRONT);

    glBindBuffer(GL_PIXEL_PACK_BUFFER, pboIds[pboIndex]);
    glReadPixels(
        0, 0,
        imageWidth, imageHeight,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        nullptr);

    glBindBuffer(GL_PIXEL_PACK_BUFFER, pboIds[nextIndex]);
    GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);

    if (ptr) {
        std::memcpy(
            (void*)buffer,
            ptr,
            imageWidth * imageHeight * imageChannels);
        glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
    }

    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}
