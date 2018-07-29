#include "ScreenQuad.h"

ScreenQuad::ScreenQuad(GLuint width, GLuint height, Shader fragmentShader) {

    this->width = width;
    this->height = height;

    Shader vertexShader("shaders/ScreenQuadVertex.glsl", GL_VERTEX_SHADER);

    shaderProgram = std::make_shared<ShaderProgram>(vertexShader, fragmentShader);

    glGenVertexArrays(1, &vaoId);
    glGenBuffers(1, &vboId);

    const float quadVerticesAndTextureCoords [] = {
        -1.0f, -1.0f, 0.0f,   0.0f, 0.0f,
         1.0f, -1.0f, 0.0f,   1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,   0.0f, 1.0f,
        -1.0f,  1.0f, 0.0f,   0.0f, 1.0f,
         1.0f, -1.0f, 0.0f,   1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,   1.0f, 1.0f
    };

    glBindVertexArray(vaoId);
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(quadVerticesAndTextureCoords),
        quadVerticesAndTextureCoords,
        GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        5 * sizeof(float),
        0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,
        2,
        GL_FLOAT,
        GL_FALSE,
        5 * sizeof(float),
        (const void*)(3 * sizeof(float)));

    glBindVertexArray(0);
}

ScreenQuad::ScreenQuad(GLuint width, GLuint height, std::string fragmentShaderPath) 
    : ScreenQuad(width, height, Shader(fragmentShaderPath, GL_FRAGMENT_SHADER)) {

}

ScreenQuad::~ScreenQuad() {

    glDeleteVertexArrays(1, &vaoId);
    glDeleteBuffers(1, &vboId);
}

void ScreenQuad::render(std::function<void(GLuint)> renderFunction) {

    renderFunction(shaderProgram->id);

    glBindVertexArray(vaoId);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
