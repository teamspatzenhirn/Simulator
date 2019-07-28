#include <iostream>

#include "Model.h"

#include "Storage.h"

Model::Model() : Model(GL_STATIC_DRAW) {
}

Model::Model(GLenum storageType) : storageType(storageType) {

    glGenVertexArrays(1, &vaoId);
    glGenBuffers(1, &vboId);
}

Model::Model(const Model& model) {

    glGenVertexArrays(1, &vaoId);
    glGenBuffers(1, &vboId);

    storageType = model.storageType;
    subModels = model.subModels;
    material = model.material;
    vertices = model.vertices;
    boundingBox = model.boundingBox;

    upload();
}

Model::Model(std::string path) : Model(path, GL_STATIC_DRAW) {
}

Model::Model(std::string path, GLenum storageType) : storageType(storageType) {

    glGenVertexArrays(1, &vaoId);
    glGenBuffers(1, &vboId);

    if (!load(*this, path)) {
        std::cerr << "Could not load model from " << path << std::endl;
        std::exit(-1);
    }
}

Model::~Model() {

    glDeleteVertexArrays(1, &vaoId);
    glDeleteBuffers(1, &vboId);
}

void Model::updateBoundingBox() {

    glm::vec3 bboxMins;
    glm::vec3 bboxMaxs;

    // initializing bboxMins and bboxMax 

    if (vertices.size() > 0) {

        Vertex& q = vertices.at(0);

        bboxMins = q.position;
        bboxMaxs = bboxMins;

        for (Vertex& v : vertices) {

            bboxMins.x = std::min(bboxMins.x, v.position.x);
            bboxMins.y = std::min(bboxMins.y, v.position.y);
            bboxMins.z = std::min(bboxMins.z, v.position.z);

            bboxMaxs.x = std::max(bboxMaxs.x, v.position.x);
            bboxMaxs.y = std::max(bboxMaxs.y, v.position.y);
            bboxMaxs.z = std::max(bboxMaxs.z, v.position.z);
        }

    } else if (subModels.size() > 0) {
    
        Model& m = subModels.at(0);

        m.updateBoundingBox();

        bboxMins = m.boundingBox.center - m.boundingBox.size / 2.0f;
        bboxMaxs = m.boundingBox.center + m.boundingBox.size / 2.0f;
    }

    // incorporating the bounding boxes of sub models

    for (Model& m: subModels) {

        glm::vec3 bmin = m.boundingBox.center - m.boundingBox.size / 2.0f;
        glm::vec3 bmax = m.boundingBox.center + m.boundingBox.size / 2.0f;

        bboxMins.x = std::min(bboxMins.x, bmin.x);
        bboxMins.y = std::min(bboxMins.y, bmin.y);
        bboxMins.z = std::min(bboxMins.z, bmin.z);

        bboxMaxs.x = std::max(bboxMaxs.x, bmax.x);
        bboxMaxs.y = std::max(bboxMaxs.y, bmax.y);
        bboxMaxs.z = std::max(bboxMaxs.z, bmax.z);
    }

    boundingBox.size = bboxMaxs - bboxMins;
    boundingBox.center = bboxMins + boundingBox.size / 2.0f;
}

void Model::upload(GLuint positionLocation,
                   GLuint normalLocation,
                   GLuint texCoordLocation) {

    glBindVertexArray(vaoId);

    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size() * sizeof(Vertex),
        vertices.data(),
        storageType);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        positionLocation,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        normalLocation,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (const void*)12);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
        texCoordLocation,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (const void*)24);

    glBindVertexArray(0);

    for (Model& m : subModels) {
        m.upload(positionLocation, normalLocation, texCoordLocation);
    }
}

void Model::renderMaterialAndVertices(GLuint shaderProgramId) {

    // phong coefficients for ambient, diffuse and specular shading
    GLint kaLocation = glGetUniformLocation(shaderProgramId, "ka");
    glUniform3f(kaLocation, material.ka.x, material.ka.y, material.ka.z);

    GLint kdLocation = glGetUniformLocation(shaderProgramId, "kd");
    glUniform3f(kdLocation, material.kd.x, material.kd.y, material.kd.z);

    GLint ksLocation = glGetUniformLocation(shaderProgramId, "ks");
    glUniform3f(ksLocation, material.ks.x, material.ks.y, material.ks.z);

    // specular exponent
    GLint nsLocation = glGetUniformLocation(shaderProgramId, "ns");
    glUniform1f(nsLocation, material.ns);

    glBindVertexArray(vaoId);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertices.size());
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

    renderMaterialAndVertices(shaderProgramId);

    for (Model& m : subModels) {
        m.renderMaterialAndVertices(shaderProgramId);
    }
}
