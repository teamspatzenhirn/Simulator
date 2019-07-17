#ifndef INC_2019_MARKER_H
#define INC_2019_MARKER_H

#include <vector>
#include <memory>

#define _USE_MATH_DEFINES
#include <cmath>

#define GLM_ENABLE_EXPERIMENTAL 
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/io.hpp>

#include "scene/ModelStore.h"

#include "scene/Scene.h"

class MarkerModule {

    struct RestrictedPose {

        Pose* pose;

        // 1 = enabled, 0 = disabled
        int transformRestriction;
    };
    
    int selectedTransformRestriction;

    std::vector<RestrictedPose> modelPoses;

    struct {
        float x;
        float y;
        bool click;
        bool pressed;
        glm::vec3 clickRay;
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

    bool hasSelection(Scene::Selection& selection);
    void deselect(Scene::Selection& selection);

    bool isTransformAllowed(
            int transformRestriction,
            SelectionMode selectionMode,
            Axis axis);

    void updateMouseState(GLFWwindow* window, Camera& camera);

    void updateSelectionState(
            Camera& camera,
            Scene::Selection& selection);

    void updateModifiers(
            Camera& camera,
            Scene::Selection& selection);

    void renderMarkers(
            GLuint shaderProgramId,
            Model& markerModel,
            glm::vec3& cameraPosition,
            Scene::Selection& selection);

    void renderModifiers(
            GLuint shaderProgramId,
            ModelStore& modelStore,
            glm::vec3& cameraPosition,
            Scene::Selection& selection);

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

    void update(
            GLFWwindow* window,
            Camera& camera,
            Scene::Selection& selection);

    void render(
            GLuint shaderProgramId,
            ModelStore& modelStore,
            Camera& cameraMatrix,
            Scene::Selection& selection);
};

#endif
