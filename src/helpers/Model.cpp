#include "Model.h"

Model::Model(std::string path) : Model(path, GL_STATIC_DRAW) {
}

Model::Model(std::string path, GLenum storageType) : storageType(storageType) {

    glGenVertexArrays(1, &vaoId);
    glGenBuffers(1, &vboId);

    objl::Loader loader;

    if (!loader.LoadFile(path)) {
        std::cout << "Model " << path << " could not be loaded!";
        std::exit(-1);
    }

    if (loader.LoadedMeshes.size() < 1) {
        std::cout << "Model " << path << " contains no data!";
        std::exit(-1);
    }

    objl::Mesh mesh = loader.LoadedMeshes.at(0);

    // TODO: handle multiple matrials properly

    for (objl::Mesh& mesh : loader.LoadedMeshes) {
        vertices.insert(vertices.end(), mesh.Vertices.begin(), mesh.Vertices.end());
        material = mesh.MeshMaterial;
    }
}

Model::~Model() {

    glDeleteVertexArrays(1, &vaoId);
    glDeleteBuffers(1, &vboId);
}

void Model::upload(GLuint positionLocation,
                   GLuint normalLocation,
                   GLuint texCoordLocation) {

    glBindVertexArray(vaoId);

    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size() * sizeof(objl::Vertex),
        vertices.data(),
        storageType);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        positionLocation,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(objl::Vertex),
        0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        normalLocation,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(objl::Vertex),
        (const void*)12);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
        texCoordLocation,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(objl::Vertex),
        (const void*)24);

    glBindVertexArray(0);
}

void Model::render(GLuint shaderProgramId, glm::mat4 modelMatrix) {

    GLint modelLocation = glGetUniformLocation(shaderProgramId, "model");
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &modelMatrix[0][0]);

    glm::mat3 normalMat =
        glm::mat3(glm::transpose(glm::inverse(modelMatrix)));
    GLint normalMatLocation =
        glGetUniformLocation(shaderProgramId, "normalMat");
    glUniformMatrix3fv(normalMatLocation, 1, GL_FALSE, &normalMat[0][0]);

    // phong coefficients for ambient, diffuse and specular shading
    GLint kaLocation = glGetUniformLocation(shaderProgramId, "ka");
    glUniform3f(kaLocation, material.Ka.X, material.Ka.Y, material.Ka.Z);

    GLint kdLocation = glGetUniformLocation(shaderProgramId, "kd");
    glUniform3f(kdLocation, material.Kd.X, material.Kd.Y, material.Kd.Z);

    GLint ksLocation = glGetUniformLocation(shaderProgramId, "ks");
    glUniform3f(ksLocation, material.Ks.X, material.Ks.Y, material.Ks.Z);

    // specular exponent
    GLint nsLocation = glGetUniformLocation(shaderProgramId, "ns");
    glUniform1f(nsLocation, material.Ns);

    glBindVertexArray(vaoId);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
}
