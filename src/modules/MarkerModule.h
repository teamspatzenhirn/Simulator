#ifndef INC_2019_MARKER_H
#define INC_2019_MARKER_H

#include <vector>
#include <string>
#include <memory>
#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "helpers/Helpers.h"

class MarkerModule {

    std::vector<glm::mat4*> modelMatrices;

    std::shared_ptr<Model> markerModel;

public:

    MarkerModule();

    void addMarker(glm::mat4& model);
    void removeMarker(glm::mat4& model);

    void render(GLFWwindow* window, GLuint shaderProgramId, Camera& cameraMatrix);
};

#endif
