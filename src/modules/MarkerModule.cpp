#include "MarkerModule.h"

MarkerModule::MarkerModule() { 

    markerModel = std::make_shared<Model>("models/marker.obj");
    arrowModel = std::make_shared<Model>("models/arrow.obj");
    scaleArrowModel = std::make_shared<Model>("models/scale_arrow.obj");
    ringModel = std::make_shared<Model>("models/ring.obj");
}

float MarkerModule::getScale(glm::vec3& cameraPosition, glm::vec3& modelPosition) {
    
    return glm::length(cameraPosition - modelPosition) * 0.05;
}

bool MarkerModule::hasSelection(Scene::Selection& selection) {

    if (selection.pose == nullptr) {
        return false;
    } else {
        for (const RestrictedPose& p : modelPoses) {
            if (p.pose == selection.pose) {
                return true;
            }
        }

        return false;
    }
}

void MarkerModule::deselect(Scene::Selection& selection) {

    selection.pose = nullptr;
    selectedTransformRestriction = ALL;
    selectionMode = TRANSLATE;
}

bool MarkerModule::isTransformAllowed(int transformRestriction, SelectionMode selectionMode, Axis axis) {

    switch (axis) {
        case X_AXIS:
            switch (selectionMode) {
                case TRANSLATE:
                    return (transformRestriction & TRANSLATE_X) > 0;
                case SCALE:
                    return (transformRestriction & SCALE_X) > 0;
                case ROTATE:
                    return (transformRestriction & ROTATE_X) > 0;
            }
        case Y_AXIS:
            switch (selectionMode) {
                case TRANSLATE:
                    return (transformRestriction & TRANSLATE_Y) > 0;
                case SCALE:
                    return (transformRestriction & SCALE_Y) > 0;
                case ROTATE:
                    return (transformRestriction & ROTATE_Y) > 0;
            }
        case Z_AXIS:
            switch (selectionMode) {
                case TRANSLATE:
                    return (transformRestriction & TRANSLATE_Z) > 0;
                case SCALE:
                    return (transformRestriction & SCALE_Z) > 0;
                case ROTATE:
                    return (transformRestriction & ROTATE_Z) > 0;
            }
        case NONE:
            return false;
    }

    return false;
}

void MarkerModule::updateMouseState(GLFWwindow* window, Camera& camera) {

    double mouseX;
    double mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    mouse.x = (float)mouseX;
    mouse.y = (float)mouseY;

    mouse.click = false;

    for (MouseButtonEvent& evt : getMouseButtonEvents()) {
        if (evt.action == GLFW_PRESS && evt.button == GLFW_MOUSE_BUTTON_LEFT) {
            mouse.click = true;
        }
    }

    mouse.pressed = GLFW_PRESS == getMouseButton(GLFW_MOUSE_BUTTON_LEFT);

    if (mouse.pressed) {
        int windowWidth;
        int windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);
        mouse.clickRay = camera.pickRay(
                mouse.x, mouse.y, windowWidth, windowHeight);
    }
}

void MarkerModule::updateSelectionState(Camera& camera, Scene::Selection& selection) {

    glm::mat4 view = camera.pose.getInverseMatrix();

    glm::vec3 eyeVector = glm::normalize(glm::vec3(
            view[0][2], view[1][2], view[2][2]));

    glm::vec3 cameraPosition = camera.pose.position;

    for (const RestrictedPose& pose : modelPoses) {

        float scale = getScale(cameraPosition, pose.pose->position);

        glm::vec3 intersectionPoint = intersectLineWithPlane(
                mouse.clickRay, cameraPosition, eyeVector, pose.pose->position);

        if (mouse.click) {

            if (glm::length(pose.pose->position - intersectionPoint) < 0.5f * scale) {

                if (selection.pose == pose.pose) {

                    // finally! i can use a do while loop!
                    // *tears of joy* 
                    do {
                        selectionMode = (SelectionMode)((selectionMode + 1) % 3);
                    } while (!isTransformAllowed(
                                selectedTransformRestriction,
                                selectionMode,
                                X_AXIS) && 
                           !isTransformAllowed(
                                selectedTransformRestriction,
                                selectionMode,
                                Y_AXIS) &&
                           !isTransformAllowed(
                                selectedTransformRestriction,
                                selectionMode,
                                Z_AXIS) &&
                            selectionMode != TRANSLATE);
                } else {
                    selectionMode = TRANSLATE;
                }

                selection.handled = true;
                selection.pose = pose.pose;
                selectedTransformRestriction = pose.transformRestriction;
            }
        }

        if (selection.pose == pose.pose) {
            selectedTransformRestriction = pose.transformRestriction;
        }
    }
}

void MarkerModule::updateModifiers(Camera& camera, Scene::Selection& selection) {

    if (!mouse.pressed) {
        return;
    }

    glm::vec3 cameraPosition = camera.pose.position;

    if (mouse.click) {
        mod.dragStartModelPosition = selection.pose->position;
        mod.dragStartScale = getScale(cameraPosition, mod.dragStartModelPosition);

        if (selectionMode == TRANSLATE || selectionMode == SCALE) {

            float arrowLength = 4.5f * mod.dragStartScale;
            float arrowHeight = 0.5f * mod.dragStartScale;

            glm::vec3 xLocalPoint = mousePosInArrowCoords(camera, X_AXIS);
            glm::vec3 yLocalPoint = mousePosInArrowCoords(camera, Y_AXIS);
            glm::vec3 zLocalPoint = mousePosInArrowCoords(camera, Z_AXIS);

            if (xLocalPoint.x < arrowLength && xLocalPoint.x > 0
                    && std::abs(xLocalPoint.y) < arrowHeight
                    && isTransformAllowed(selectedTransformRestriction, selectionMode, X_AXIS)) {
                selection.handled = true;
                selectedAxis = X_AXIS;
            } else if (yLocalPoint.x < arrowLength && yLocalPoint.x > 0
                    && std::abs(yLocalPoint.y) < arrowHeight
                    && isTransformAllowed(selectedTransformRestriction, selectionMode, Y_AXIS)) {
                selection.handled = true;
                selectedAxis = Y_AXIS;
            } else if (zLocalPoint.x < arrowLength && zLocalPoint.x > 0
                    && std::abs(zLocalPoint.y) < arrowHeight
                    && isTransformAllowed(selectedTransformRestriction, selectionMode, Z_AXIS)) {
                selection.handled = true;
                selectedAxis = Z_AXIS;
            } else {
                selectedAxis = NONE;
            }

            mod.prevDragState.x = selectedAxis == X_AXIS ? xLocalPoint.x : 0;
            mod.prevDragState.y = selectedAxis == Y_AXIS ? yLocalPoint.x : 0;
            mod.prevDragState.z = selectedAxis == Z_AXIS ? zLocalPoint.x : 0;
        } else {
            float innerRadius = 2.3f * mod.dragStartScale;
            float outerRadius = 3.3f * mod.dragStartScale;

            glm::vec3 xLocalPoint = mousePosInRotateCoords(camera, X_AXIS);
            glm::vec3 yLocalPoint = mousePosInRotateCoords(camera, Y_AXIS);
            glm::vec3 zLocalPoint = mousePosInRotateCoords(camera, Z_AXIS);

            float xLength = glm::length(glm::vec2(xLocalPoint.x, xLocalPoint.y));
            float yLength = glm::length(glm::vec2(yLocalPoint.x, yLocalPoint.y));
            float zLength = glm::length(glm::vec2(zLocalPoint.x, zLocalPoint.y));

            if (innerRadius < xLength && xLength < outerRadius
                    && isTransformAllowed(selectedTransformRestriction, selectionMode, X_AXIS)) {
                selection.handled = true;
                selectedAxis = X_AXIS;
            } else if (innerRadius < yLength && yLength < outerRadius
                    && isTransformAllowed(selectedTransformRestriction, selectionMode, Y_AXIS)) {
                selection.handled = true;
                selectedAxis = Y_AXIS;
            } else if (innerRadius < zLength && zLength < outerRadius
                    && isTransformAllowed(selectedTransformRestriction, selectionMode, Z_AXIS)) {
                selection.handled = true;
                selectedAxis = Z_AXIS;
            } else {
                selectedAxis = NONE;
            }

            float xAngle = std::atan2(xLocalPoint.x, xLocalPoint.y);
            float yAngle = std::atan2(yLocalPoint.x, yLocalPoint.y);
            float zAngle = std::atan2(zLocalPoint.x, zLocalPoint.y);

            mod.prevDragState.x = selectedAxis == X_AXIS ? xAngle : 0;
            mod.prevDragState.y = selectedAxis == Y_AXIS ? yAngle : 0;
            mod.prevDragState.z = selectedAxis == Z_AXIS ? zAngle : 0;
        }
    } else {
        glm::vec3 dragState;

        if (selectionMode == TRANSLATE || selectionMode == SCALE) {
            dragState.x = selectedAxis == X_AXIS ?
                mousePosInArrowCoords(camera, X_AXIS).x : 0;
            dragState.y = selectedAxis == Y_AXIS ?
                mousePosInArrowCoords(camera, Y_AXIS).x : 0;
            dragState.z = selectedAxis == Z_AXIS ?
                mousePosInArrowCoords(camera, Z_AXIS).x : 0;
        } else {
            glm::vec3 xLocalPoint = mousePosInRotateCoords(camera, X_AXIS);
            glm::vec3 yLocalPoint = mousePosInRotateCoords(camera, Y_AXIS);
            glm::vec3 zLocalPoint = mousePosInRotateCoords(camera, Z_AXIS);

            float xAngle = std::atan2(xLocalPoint.x, xLocalPoint.y);
            float yAngle = std::atan2(yLocalPoint.x, yLocalPoint.y);
            float zAngle = std::atan2(zLocalPoint.x, zLocalPoint.y);

            dragState.x = selectedAxis == X_AXIS ? xAngle : 0;
            dragState.y = selectedAxis == Y_AXIS ? yAngle : 0;
            dragState.z = selectedAxis == Z_AXIS ? zAngle : 0;
        }

        switch(selectionMode) {
            case TRANSLATE: {
                glm::vec3 translation = dragState - mod.prevDragState;
                selection.pose->position += translation;
                break;
            }
            case SCALE: {
                glm::vec3 scale = dragState - mod.prevDragState;
                selection.pose->scale += scale;
                break;
            }
            case ROTATE: {
                glm::vec3 rotation = dragState - mod.prevDragState;

                glm::vec3 axis(1.0f, 0.0f, 0.0f);
                float angle = 0.0f;

                switch (selectedAxis) {
                    case X_AXIS:
                        axis = glm::vec3(1, 0, 0);
                        angle = rotation.x;
                        break;
                    case Y_AXIS:
                        axis = glm::vec3(0, 1, 0);
                        angle = rotation.y;
                        break;
                    case Z_AXIS:
                        axis = glm::vec3(0, 0, 1);
                        angle = rotation.z;
                        break;
                    case NONE:
                        break;
                }

                glm::mat4 invModelMat = glm::inverse(
                        glm::mat4_cast(selection.pose->rotation));
                axis = glm::normalize(glm::vec3(
                            invModelMat * glm::vec4(axis, 0)));

                selection.pose->rotation = glm::rotate(
                        selection.pose->rotation, angle, axis);
                break;
            }
        }

        mod.prevDragState = dragState;
    }
}

void MarkerModule::renderMarkers(GLuint shaderProgramId, glm::vec3& cameraPosition, Scene::Selection& selection) {

    GLuint billboardLocation =
        glGetUniformLocation(shaderProgramId, "billboard");
    glUniform1i(billboardLocation, true);

    for (const RestrictedPose& pose : modelPoses) {

        float scale = getScale(cameraPosition, pose.pose->position);

        glm::mat4 markerMat = glm::mat4(1.0f);
        markerMat = glm::translate(markerMat, pose.pose->position);
        markerMat = glm::scale(markerMat, glm::vec3(scale, scale, scale));

        if (selection.pose == pose.pose) {
            markerModel->material.Ka = objl::Vector3(1.0f, 1.0f, 0.0f);
            markerModel->material.Kd = objl::Vector3(1.0f, 1.0f, 0.0f);
            markerModel->material.Ks = objl::Vector3(0.0f, 0.0f, 0.0f);
            markerModel->render(shaderProgramId, markerMat);
        } else {
            markerModel->material.Ka = objl::Vector3(0.0f, 1.0f, 0.0f);
            markerModel->material.Kd = objl::Vector3(0.0f, 1.0f, 0.0f);
            markerModel->material.Ks = objl::Vector3(0.0f, 0.0f, 0.0f);
            markerModel->render(shaderProgramId, markerMat);
        }
    }

    glUniform1i(billboardLocation, false);
}

void MarkerModule::renderModifiers(GLuint shaderProgramId, glm::vec3& cameraPosition, Scene::Selection& selection) {
    
    float scale = getScale(cameraPosition, selection.pose->position);
    float offset = 1.0f;
    std::shared_ptr<Model> model = arrowModel;

    switch(selectionMode) {
        case TRANSLATE:
            model = arrowModel;
            break;
        case SCALE: 
            model = scaleArrowModel;
            break;
        case ROTATE:
            model = ringModel;
            scale *= 3;
            offset = 0.0f;
            break;
    }

    if (isTransformAllowed(
                selectedTransformRestriction,
                selectionMode,
                Y_AXIS)) {

        glm::mat4 upMat = glm::mat4(1.0f);
        upMat = glm::translate(upMat, selection.pose->position);
        upMat = glm::translate(upMat, glm::vec3(0.0f, offset * scale, 0.0f));
        upMat = glm::scale(upMat, glm::vec3(scale, scale, scale));

        model->material.Ka = objl::Vector3(0.0f, 1.0f, 0.0f);
        model->material.Kd = objl::Vector3(0.0f, 1.0f, 0.0f);
        model->material.Ks = objl::Vector3(0.0f, 1.0f, 0.0f);

        model->render(shaderProgramId, upMat);
    }

    if (isTransformAllowed(
                selectedTransformRestriction,
                selectionMode,
                X_AXIS)) {

        model->material.Ka = objl::Vector3(1.0f, 0.0f, 0.0f);
        model->material.Kd = objl::Vector3(1.0f, 0.0f, 0.0f);
        model->material.Ks = objl::Vector3(1.0f, 0.0f, 0.0f);

        glm::mat4 rightMat = glm::mat4(1.0f);
        rightMat = glm::translate(rightMat, selection.pose->position);
        rightMat = glm::rotate(rightMat,
                -(float)M_PI / 2.0f, glm::vec3(0.0f, 0.0f, 1.0f));
        rightMat = glm::translate(rightMat, glm::vec3(0.0f, offset * scale, 0.0f));
        rightMat = glm::scale(rightMat, glm::vec3(scale, scale, scale));

        model->render(shaderProgramId, rightMat);
    }

    if (isTransformAllowed(
                selectedTransformRestriction,
                selectionMode,
                Z_AXIS)) {

        model->material.Ka = objl::Vector3(0.0f, 0.0f, 1.0f);
        model->material.Kd = objl::Vector3(0.0f, 0.0f, 1.0f);
        model->material.Ks = objl::Vector3(0.0f, 0.0f, 1.0f);

        glm::mat4 forwardMat = glm::mat4(1.0f);
        forwardMat = glm::translate(forwardMat, selection.pose->position);
        forwardMat = glm::rotate(forwardMat,
                (float)M_PI / 2.0f, glm::vec3(1.0f, 0.0f, 0.0f));
        forwardMat = glm::translate(forwardMat, glm::vec3(0.0f, offset * scale, 0.0f));
        forwardMat = glm::scale(forwardMat, glm::vec3(scale, scale, scale));

        model->render(shaderProgramId, forwardMat);
    }
}

void MarkerModule::add(Pose& pose, int transformRestriction) {

    for (const RestrictedPose& p : modelPoses) {
        if (p.pose == &pose) {
            return;
        }
    }

    modelPoses.push_back(RestrictedPose{&pose, transformRestriction});
}

void MarkerModule::update(
        GLFWwindow* window,
        Camera& camera,
        Scene::Selection& selection) {

    updateMouseState(window, camera);
    updateSelectionState(camera, selection);

    if (hasSelection(selection)) {
        updateModifiers(camera, selection);
    } else {
        deselect(selection);
    }

    for (KeyEvent& e : getKeyEvents()) {
        if (GLFW_KEY_ESCAPE == e.key && GLFW_PRESS == e.action) {
            deselect(selection);
        }
    }

    if (selection.handled) {
        clearMouseInput();
    } else {
        deselect(selection);
    }

    modelPoses.clear();
}

void MarkerModule::render(
        GLFWwindow* window,
        GLuint shaderProgramId,
        Camera& camera,
        Scene::Selection& selection) {

    GLuint lightingLocation =
        glGetUniformLocation(shaderProgramId, "lighting");
    glUniform1i(lightingLocation, false);

    glm::vec3 cameraPosition = camera.pose.position;

    if (hasSelection(selection)) {
        renderModifiers(shaderProgramId, cameraPosition, selection);
    } 

    renderMarkers(shaderProgramId, cameraPosition, selection);

    glUniform1i(lightingLocation, true);
}

glm::vec3 MarkerModule::intersectLineWithPlane(
        glm::vec3& lineDirection,
        glm::vec3& linePoint,
        glm::vec3& planeNormal,
        glm::vec3& planePoint) {

    float t = glm::dot(planePoint - linePoint, planeNormal);
    t /= glm::dot(lineDirection, planeNormal);

    return linePoint + lineDirection * t;
}

glm::vec3 MarkerModule::intersectionPointInPlaneCoord(
        glm::vec3& lineDirection,
        glm::vec3& linePoint,
        glm::vec3& planeNormal,
        glm::vec3& planePoint,
        glm::vec3& planeRightVector) {

    glm::vec3 intersectionPoint = intersectLineWithPlane(
            lineDirection,
            linePoint,
            planeNormal,
            planePoint);

    glm::vec3 planeUpVector = glm::cross(planeNormal, planeRightVector);

    glm::mat3 toPlaneCoords = glm::transpose(
            glm::mat3(planeRightVector, planeUpVector, planeNormal));

    glm::vec3 localPoint = toPlaneCoords * (intersectionPoint - planePoint);

    return localPoint;
}

glm::vec3 MarkerModule::mousePosInArrowCoords(Camera& camera, Axis axis) {

    glm::vec3 cameraPosition = camera.pose.position;
    glm::vec3 normal = cameraPosition - mod.dragStartModelPosition;
    glm::vec3 right;

    switch (axis) {
        case X_AXIS:
            normal = glm::normalize(glm::vec3(0.0f, normal.y, normal.z));
            right = glm::vec3(1.0f, 0.0f, 0.0f);
            break;
        case Y_AXIS:
            normal = glm::normalize(glm::vec3(normal.x, 0.0f, normal.z));
            right = glm::vec3(0.0f, 1.0f, 0.0f);
            break;
        case Z_AXIS:
            normal = glm::normalize(glm::vec3(normal.x, normal.y, 0.0f));
            right = glm::vec3(0.0f, 0.0f, 1.0f);
            break;
        case NONE:
            return glm::vec3(0.0f, 0.0f, 0.0f);
    }

    glm::vec3 localPoint = intersectionPointInPlaneCoord(
            mouse.clickRay,
            cameraPosition,
            normal,
            mod.dragStartModelPosition,
            right);

    return localPoint;
}

glm::vec3 MarkerModule::mousePosInRotateCoords(Camera& camera, Axis axis) {

    glm::vec3 normal;
    glm::vec3 right;

    switch (axis) {
        case X_AXIS:
            normal = glm::vec3(-1.0f, 0.0f, 0.0f);
            right = glm::vec3(0.0f, 0.0f, 1.0f);
            break;
        case Y_AXIS:
            normal = glm::vec3(0.0f, -1.0f, 0.0f);
            right = glm::vec3(1.0f, 0.0f, 0.0f);
            break;
        case Z_AXIS:
            normal = glm::vec3(0.0f, 0.0f, -1.0f);
            right = glm::vec3(1.0f, 0.0f, 0.0f);
            break;
        case NONE:
            return glm::vec3(0.0f, 0.0f, 0.0f);
    }

    glm::vec3 cameraPosition = camera.pose.position;

    glm::vec3 localPoint = intersectionPointInPlaneCoord(
            mouse.clickRay,
            cameraPosition,
            normal,
            mod.dragStartModelPosition,
            right);

    return localPoint;
}
