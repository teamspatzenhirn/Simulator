#include "Model.h"

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

    objl::Loader loader;

    if (!loader.LoadFile(path)) {
        std::cout << "Model " << path << " could not be loaded!";
        std::exit(-1);
    }

    if (loader.LoadedMeshes.size() < 1) {
        std::cout << "Model " << path << " contains no data!";
        std::exit(-1);
    }

    if (loader.LoadedMeshes.size() == 1) {
        objl::Mesh mesh = loader.LoadedMeshes.back();
        material = mesh.MeshMaterial;
        vertices = mesh.Vertices;
    } else {
        for (objl::Mesh& m : loader.LoadedMeshes) {
            subModels.emplace_back(storageType);
            Model& model = subModels.back();
            model.material = m.MeshMaterial;
            model.vertices = m.Vertices;
        }
    }

    upload();
    updateBoundingBox();
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

        objl::Vertex& q = vertices.at(0);

        bboxMins = glm::vec3(q.Position.X, q.Position.Y, q.Position.Z);
        bboxMaxs = bboxMins;

        for (objl::Vertex& v : vertices) {

            bboxMins.x = std::min(bboxMins.x, v.Position.X);
            bboxMins.y = std::min(bboxMins.y, v.Position.Y);
            bboxMins.z = std::min(bboxMins.z, v.Position.Z);

            bboxMaxs.x = std::max(bboxMaxs.x, v.Position.X);
            bboxMaxs.y = std::max(bboxMaxs.y, v.Position.Y);
            bboxMaxs.z = std::max(bboxMaxs.z, v.Position.Z);
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

    for (Model& m : subModels) {
        m.upload(positionLocation, normalLocation, texCoordLocation);
    }
}

void Model::renderMaterialAndVertices(GLuint shaderProgramId) {

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
