#ifndef INC_2019_MARKER_H
#define INC_2019_MARKER_H

#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <cstdint>

#define _USE_MATH_DEFINES
#include <cmath>

#define GLM_ENABLE_EXPERIMENTAL 
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/io.hpp>

#include "imgui/imgui.h"
#include "helpers/Helpers.h"

class MarkerModule {

    struct RestrictedPose {

        Pose* pose;

        // 1 = enabled, 0 = disabled
        int transformRestriction;

    } selectedModelPose;

    std::vector<RestrictedPose> modelPoses;

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
        glm::vec3 clickRay;
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

    enum SelectionMode {
        TRANSLATE = 0,
        SCALE = 1,
        ROTATE = 2
    } selectionMode;

    float getScale(glm::vec3& cameraPosition, glm::vec3& modelPosition);

    bool hasSelection();
    void deselect();

    bool isTransformAllowed(int transformRestriction, SelectionMode selectionMode, Axis axis);

    void updateMouseState(GLFWwindow* window, Camera& camera);

    void updateSelectionState(Camera& camera);

    void updateModifiers(Camera& camera);

    void renderMarkers(GLuint shaderProgramId, glm::vec3& cameraPosition);

    void renderModifiers(GLuint shaderProgramId, glm::vec3& cameraPosition);

    void renderGlyphTriplet(
            GLuint shaderProgramId,
            std::shared_ptr<Model>& model,
            RestrictedPose& restrictedPose,
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

    // Constants for transform restriction
    static const int TRANSLATE_X = 1;
    static const int TRANSLATE_Y = 2;
    static const int TRANSLATE_Z = 4;
    static const int TRANSLATE_ALL = TRANSLATE_X | TRANSLATE_Y | TRANSLATE_Z;

    static const int SCALE_X = 8;
    static const int SCALE_Y = 16;
    static const int SCALE_Z = 32;
    static const int SCALE_ALL = SCALE_X | SCALE_Y | SCALE_Z;

    static const int ROTATE_X = 64;
    static const int ROTATE_Y = 128;
    static const int ROTATE_Z = 256;
    static const int ROTATE_ALL = ROTATE_X | ROTATE_Y | ROTATE_Z;

    static const int ALL = TRANSLATE_ALL | SCALE_ALL | ROTATE_ALL;

    MarkerModule();

    void add(Pose& modelPose, int transformRestriction);

    Pose* getSelection();

    void update(GLFWwindow* window, Camera& camera);

    void render(GLFWwindow* window,
            GLuint shaderProgramId,
            Camera& cameraMatrix);
};

#endif
