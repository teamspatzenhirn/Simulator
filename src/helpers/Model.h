#ifndef INC_2019_MODEL_H
#define INC_2019_MODEL_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "ObjLoader.h"

class Model {

    GLuint vaoId;
    GLuint vboId;

    GLenum storageType;

    void renderMaterialAndVertices(GLuint shaderProgramId);

public:

    Model();
    Model(const Model& model);
    Model(GLenum storageType);
    Model(std::string path);
    Model(std::string path, GLenum storageType);
    ~Model();

    std::vector<Model> subModels;

    objl::Material material;
    std::vector<objl::Vertex> vertices;

    struct BoundingBox {

        glm::vec3 center{0, 0, 0};
        glm::vec3 size{0, 0, 0};

    } boundingBox;

    void updateBoundingBox();

    void upload(GLuint positionLocation = 0,
                GLuint normalLocation = 1,
                GLuint texCoordLocation = 2);

    void render(GLuint shaderProgramId, glm::mat4 modelMatrix);
};

#endif
