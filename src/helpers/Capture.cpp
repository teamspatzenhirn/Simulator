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

bool Capture::capture(GLubyte* buffer, GLenum mode) {

    pboIndex = (pboIndex + 1) % 2;
    int nextIndex = (pboIndex + 1) % 2;

    glReadBuffer(mode);

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
        const unsigned char * source = ptr;
        unsigned char * dest = buffer;
        int row;
        for(int y = 0; y < imageHeight; y++){
            row = y*imageWidth;
            for(int x = 0; x < imageWidth; x++){
                dest[row+x] = source[((row+x)*3)+x%2]; // BGBGBGBG...
            }
            y++;
            row = y*imageWidth;
            for(int x = 0; x < imageWidth; x++){
                dest[row+x] = source[((row+x)*3)+1+x%2]; // GRGRGRGRGR...
            }
        }

        glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
    }

    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

    return ptr != nullptr;
}
