#ifndef INC_2019_MARKER_H
#define INC_2019_MARKER_H

#include <vector>
#include <string>
#include <memory>
#include <algorithm>

#define _USE_MATH_DEFINES
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "helpers/Helpers.h"

class MarkerModule {

    std::vector<glm::mat4*> modelMatrices;

    std::shared_ptr<Model> markerModel;
    std::shared_ptr<Model> arrowModel;
    std::shared_ptr<Model> scaleArrowModel;
    std::shared_ptr<Model> ringModel;

    struct {
        float x;
        float y;
        bool click;
        bool pressed;
    } mouse;

    glm::vec3 prevShift;
    glm::vec3 startModelPosition;
    float startScale;
    enum SelectedAxis { X_AXIS, Y_AXIS, Z_AXIS } selectedAxis;

    glm::mat4* selectedModelMatrix;
    enum SelectionMode { TRANSLATE = 0, SCALE = 1, ROTATE = 2 } selectionMode;

    float getScale(glm::vec3& cameraPosition, glm::vec3& modelPosition);

    glm::vec3 getModelPosition(glm::mat4* modelPtr);

    bool hasSelection();

    void updateMouseState(GLFWwindow* window);

    void updateSelectionState(Camera& camera);

    void updateModifiers(Camera& camera);

    void renderMarkers(GLuint shaderProgramId, glm::vec3& cameraPosition);

    void renderModifiers(GLuint shaderProgramId, glm::vec3& cameraPosition);

    void renderGlyphTriplet(
            GLuint shaderProgramId,
            std::shared_ptr<Model>& model,
            glm::vec3 position,
            float scale, 
            float offset);

    glm::vec3 intersectLineWithPlane(
            glm::vec3& lineDirection,
            glm::vec3& linePoint,
            glm::vec3& planeNormal,
            glm::vec3& planePoint);

    glm::vec3 intersectionPointInPlaneCoord(
            glm::vec3& lineDirection,
            glm::vec3& linePoint,
            glm::vec3& planeNormal,
            glm::vec3& planePoint,
            glm::vec3& planeRightVector);

    void decomposeTransformationMatrix(
            glm::mat4& matrix,
            glm::mat4& translationMatrix,
            glm::mat4& scaleMatrix,
            glm::mat4& rotationMatrix);

    bool calcShift(
            glm::vec3& shift,
            GLFWwindow* window,
            glm::vec3& clickRay,
            glm::vec3& cameraPosition,
            glm::vec3& modelPosition,
            float scale);

public:

    MarkerModule();

    void addMarker(glm::mat4& model);

    void render(GLFWwindow* window, GLuint shaderProgramId, Camera& cameraMatrix);
};

#endif
