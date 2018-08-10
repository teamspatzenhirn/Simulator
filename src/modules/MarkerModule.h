#ifndef INC_2019_MARKER_H
#define INC_2019_MARKER_H

#include <vector>
#include <string>
#include <memory>
#include <algorithm>

#include <cmath>
#define _USE_MATH_DEFINES

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "helpers/Helpers.h"

class MarkerModule {


    std::vector<glm::mat4*> modelMatrices;

    std::shared_ptr<Model> markerModel;
    std::shared_ptr<Model> markerModelSelected;

    std::shared_ptr<Model> arrowModel;
    std::shared_ptr<Model> scaleArrowModel;
    std::shared_ptr<Model> ringModel;

    bool isClick;

    glm::mat4* selectedMarker;
    enum SelectionMode { TRANSLATE = 0, SCALE = 1, ROTATE = 2 } selectionMode;

    void renderModifiers(
            GLuint shaderProgramId,
            std::shared_ptr<Model> model,
            float scale, 
            float offset);
public:

    MarkerModule();

    void addMarker(glm::mat4& model);

    void render(GLFWwindow* window, GLuint shaderProgramId, Camera& cameraMatrix);
};

#endif
