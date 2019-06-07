#ifndef INC_2019_MODEL_H
#define INC_2019_MODEL_H

#include <string>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

class Model {

    GLuint vaoId;
    GLuint vboId;

    GLenum storageType;

    void renderMaterialAndVertices(GLuint shaderProgramId);

public:

    Model();
    Model(const Model& model);
    explicit Model(GLenum storageType);
    explicit Model(std::string path);
    Model(std::string path, GLenum storageType);
    ~Model();

    std::vector<Model> subModels;

    struct Vertex {

        glm::vec3 position;
        glm::vec2 normal;
        glm::vec2 textureCoordinate;
    };
    std::vector<Vertex> vertices;

    struct Material {

        std::string name;
        glm::vec3 ka;    
        glm::vec3 kd;
        glm::vec3 ks;
        float ns = 0.0f;
        float ni = 0.0f;
        float d = 0.0f;
        int illum = 0;
        std::string mapKa;
        std::string mapKd;
        std::string mapKs;
        std::string mapD;
        std::string mapBump;

    } material;

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
