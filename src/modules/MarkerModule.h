#ifndef INC_2019_MARKER_H
#define INC_2019_MARKER_H

#include <vector>
#include <string>
#include <memory>
#include <algorithm>

#define _USE_MATH_DEFINES
#include <cmath>

#define GLM_ENABLE_EXPERIMENTAL 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/io.hpp>

#include "helpers/Helpers.h"

class MarkerModule {

    std::vector<Pose*> modelPoses;

    std::shared_ptr<Model> markerModel;
    std::shared_ptr<Model> arrowModel;
    std::shared_ptr<Model> scaleArrowModel;
    std::shared_ptr<Model> ringModel;

    struct {
        float x;
        float y;
        bool click;
        bool pressed;
        bool handled;
        int prevButtonState = GLFW_RELEASE;
    } mouse;

    struct {
        glm::vec3 dragStartModelPosition;
        float dragStartScale;
        glm::vec3 prevDragState;
    } mod;

    enum Axis {
        X_AXIS,
        Y_AXIS,
        Z_AXIS,
        NONE,
    } selectedAxis;

    Pose* selectedModelPose;
    enum SelectionMode { TRANSLATE = 0, SCALE = 1, ROTATE = 2 } selectionMode;

    float getScale(glm::vec3& cameraPosition, glm::vec3& modelPosition);

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

    glm::vec3 mousePosInRotateCoords(Camera& camera, Axis axis);

    glm::vec3 mousePosInArrowCoords(Camera& camera, Axis axis);

public:

    MarkerModule();

    void add(Pose& modelPose);

    void render(GLFWwindow* window, GLuint shaderProgramId, Camera& cameraMatrix);
};

#endif
