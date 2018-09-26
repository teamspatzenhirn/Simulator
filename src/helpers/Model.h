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

public:

    std::vector<objl::Vertex> vertices;
    objl::Material material;

    Model();
    Model(GLenum storageType);
    Model(std::string path);
    Model(std::string path, GLenum storageType);
    ~Model();

    void upload(GLuint positionLocation = 0,
                GLuint normalLocation = 1,
                GLuint texCoordLocation = 2);

    void render(GLuint shaderProgramId, glm::mat4 modelMatrix);
};

#endif
