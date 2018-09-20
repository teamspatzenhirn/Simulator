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

bool Capture::capture(GLubyte* buffer, GLuint width, GLuint height, GLenum mode) {

    int dataSize = width * height * 3;

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
        GL_RGB,
        GL_UNSIGNED_BYTE,
        nullptr);

    glBindBuffer(GL_PIXEL_PACK_BUFFER, pboIds[nextIndex]);
    GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);

    // TODO: move this in the camera module 
    // this is not actually the purpose of the capture helper

    if (ptr) {
        const unsigned char * source = ptr;
        unsigned char * dest = buffer;
        int row;
        for(int y = 0; y < height; y++){
            row = y*width;
            for(int x = 0; x < width; x++){
                dest[row+x] = source[((row+x)*3)+x%2]; // BGBGBGBG...
            }
            y++;
            row = y*width;
            for(int x = 0; x < width; x++){
                dest[row+x] = source[((row+x)*3)+1+x%2]; // GRGRGRGRGR...
            }
        }

        glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
    }

    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

    return ptr != nullptr;
}
