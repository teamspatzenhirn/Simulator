#include "ScreenQuad.h"

ScreenQuad::ScreenQuad(Shader fragmentShader) {

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

ScreenQuad::ScreenQuad(std::string fragmentShaderPath) 
    : ScreenQuad(Shader(fragmentShaderPath, GL_FRAGMENT_SHADER)) {

}

ScreenQuad::~ScreenQuad() {

    glDeleteVertexArrays(1, &vaoId);
    glDeleteBuffers(1, &vboId);
}

GLuint ScreenQuad::start() {

    glUseProgram(shaderProgram->id);
    
    return shaderProgram->id;
}

void ScreenQuad::end() {

    glBindVertexArray(vaoId);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
