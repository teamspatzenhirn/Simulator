#include "MarkerModule.h"

MarkerModule::MarkerModule() { 

    markerModel = std::make_shared<Model>("models/marker.obj");
    markerModel->upload();

    arrowModel = std::make_shared<Model>("models/arrow.obj");
    arrowModel->upload();

    scaleArrowModel = std::make_shared<Model>("models/scale_arrow.obj");
    scaleArrowModel->upload();

    ringModel = std::make_shared<Model>("models/ring.obj");
    ringModel->upload();
}

bool MarkerModule::hasSelection() {

    auto it = std::find(
            modelPoses.begin(), modelPoses.end(), selectedModelPose);

    return it != modelPoses.end();
}

float MarkerModule::getScale(glm::vec3& cameraPosition, glm::vec3& modelPosition) {
    
    return glm::length(cameraPosition - modelPosition) * 0.05;
}

void MarkerModule::updateMouseState(GLFWwindow* window) {

    double mouseX;
    double mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    mouse.x = (float)mouseX;
    mouse.y = (float)mouseY;

    int buttonState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);

    mouse.pressed = GLFW_PRESS == buttonState;
    mouse.click = GLFW_RELEASE == mouse.prevButtonState && GLFW_PRESS == buttonState;

    if (mouse.click) {
        mouse.handled = false;
    }

    mouse.prevButtonState = buttonState;
}

void MarkerModule::updateSelectionState(Camera& camera) {

    if (!mouse.click) {
        return;
    }

    // TODO: handle width and height parameters properly
    glm::vec3 clickRay =
        camera.pickRay(mouse.x, mouse.y, 800, 600);

    glm::vec3 eyeVector = glm::normalize(glm::vec3(
            camera.view[0][2], camera.view[1][2], camera.view[2][2]));

    glm::vec3 cameraPosition = camera.getPosition();

    for (Pose* pose : modelPoses) {

        float scale = getScale(cameraPosition, pose->position);

        glm::vec3 intersectionPoint = intersectLineWithPlane(
                clickRay, cameraPosition, eyeVector, pose->position);

        if (glm::length(pose->position - intersectionPoint) < 0.5f * scale) {

            if (selectedModelPose == pose) {
                selectionMode = (SelectionMode)((selectionMode + 1) % 3);
            } else {
                selectionMode = TRANSLATE;
            }

            mouse.handled = true;
            selectedModelPose = pose;
        }
    }
}

void MarkerModule::updateModifiers(Camera& camera) {

    if (!mouse.pressed) {
        return;
    }

    glm::vec3 cameraPosition = camera.getPosition();

    if (mouse.click) {
        mod.dragStartModelPosition = selectedModelPose->position;
        mod.dragStartScale = getScale(cameraPosition, mod.dragStartModelPosition);

        if (selectionMode == TRANSLATE || selectionMode == SCALE) {

            float arrowLength = 4.5f * mod.dragStartScale;
            float arrowHeight = 0.5f * mod.dragStartScale;

            glm::vec3 xLocalPoint = mousePosInArrowCoords(camera, X_AXIS);
            glm::vec3 yLocalPoint = mousePosInArrowCoords(camera, Y_AXIS);
            glm::vec3 zLocalPoint = mousePosInArrowCoords(camera, Z_AXIS);

            if (xLocalPoint.x < arrowLength && xLocalPoint.x > 0
                    && std::abs(xLocalPoint.y) < arrowHeight) {
                mouse.handled = true;
                selectedAxis = X_AXIS;
            } else if (yLocalPoint.x < arrowLength && yLocalPoint.x > 0
                    && std::abs(yLocalPoint.y) < arrowHeight) {
                mouse.handled = true;
                selectedAxis = Y_AXIS;
            } else if (zLocalPoint.x < arrowLength && zLocalPoint.x > 0
                    && std::abs(zLocalPoint.y) < arrowHeight) {
                mouse.handled = true;
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

            if (innerRadius < xLength && xLength < outerRadius) {
                mouse.handled = true;
                selectedAxis = X_AXIS;
            } else if (innerRadius < yLength && yLength < outerRadius) {
                mouse.handled = true;
                selectedAxis = Y_AXIS;
            } else if (innerRadius < zLength && zLength < outerRadius) {
                mouse.handled = true;
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
                selectedModelPose->position += translation;
                break;
            }
            case SCALE: {
                glm::vec3 scale = dragState - mod.prevDragState;
                selectedModelPose->scale += scale;
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
                }

                glm::mat4 invModelMat = glm::inverse(
                        glm::mat4_cast(selectedModelPose->rotation));
                axis = glm::normalize(glm::vec3(
                            invModelMat * glm::vec4(axis, 0)));

                selectedModelPose->rotation = glm::rotate(
                        selectedModelPose->rotation, angle, axis);
                break;
            }
        }

        mod.prevDragState = dragState;
    }
}

void MarkerModule::renderMarkers(GLuint shaderProgramId, glm::vec3& cameraPosition) {

    GLuint billboardLocation =
        glGetUniformLocation(shaderProgramId, "billboard");
    glUniform1i(billboardLocation, true);

    for (Pose* pose : modelPoses) {

        float scale = getScale(cameraPosition, pose->position);

        glm::mat4 markerMat = glm::mat4(1.0f);
        markerMat = glm::translate(markerMat, pose->position);
        markerMat = glm::scale(markerMat, glm::vec3(scale, scale, scale));

        if (selectedModelPose == pose) {
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

void MarkerModule::renderModifiers(GLuint shaderProgramId, glm::vec3& cameraPosition) {
    
    float scale = getScale(cameraPosition, selectedModelPose->position);

    switch(selectionMode) {
        case TRANSLATE:
            renderGlyphTriplet(
                    shaderProgramId,
                    arrowModel,
                    selectedModelPose->position, 
                    scale,
                    1.0f);
            break;
        case SCALE: 
            renderGlyphTriplet(
                    shaderProgramId,
                    scaleArrowModel,
                    selectedModelPose->position,
                    scale,
                    1.0f);
            break;
        case ROTATE:
            renderGlyphTriplet(
                    shaderProgramId,
                    ringModel,
                    selectedModelPose->position,
                    scale * 3,
                    0.0f);
            break;
    }
}

void MarkerModule::renderGlyphTriplet(
            GLuint shaderProgramId,
            std::shared_ptr<Model>& model,
            glm::vec3 position,
            float scale, 
            float offset) {

    glm::mat4 upMat = glm::mat4(1.0f);
    upMat = glm::translate(upMat, position);
    upMat = glm::translate(upMat, glm::vec3(0.0f, offset * scale, 0.0f));
    upMat = glm::scale(upMat, glm::vec3(scale, scale, scale));

    model->material.Ka = objl::Vector3(0.0f, 1.0f, 0.0f);
    model->material.Kd = objl::Vector3(0.0f, 1.0f, 0.0f);
    model->material.Ks = objl::Vector3(0.0f, 1.0f, 0.0f);

    model->render(shaderProgramId, upMat);

    model->material.Ka = objl::Vector3(1.0f, 0.0f, 0.0f);
    model->material.Kd = objl::Vector3(1.0f, 0.0f, 0.0f);
    model->material.Ks = objl::Vector3(1.0f, 0.0f, 0.0f);

    glm::mat4 rightMat = glm::mat4(1.0f);
    rightMat = glm::translate(rightMat, position);
    rightMat = glm::rotate(rightMat,
            -(float)M_PI / 2.0f, glm::vec3(0.0f, 0.0f, 1.0f));
    rightMat = glm::translate(rightMat, glm::vec3(0.0f, offset * scale, 0.0f));
    rightMat = glm::scale(rightMat, glm::vec3(scale, scale, scale));

    model->render(shaderProgramId, rightMat);

    model->material.Ka = objl::Vector3(0.0f, 0.0f, 1.0f);
    model->material.Kd = objl::Vector3(0.0f, 0.0f, 1.0f);
    model->material.Ks = objl::Vector3(0.0f, 0.0f, 1.0f);

    glm::mat4 forwardMat = glm::mat4(1.0f);
    forwardMat = glm::translate(forwardMat, position);
    forwardMat = glm::rotate(forwardMat,
            (float)M_PI / 2.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    forwardMat = glm::translate(forwardMat, glm::vec3(0.0f, offset * scale, 0.0f));
    forwardMat = glm::scale(forwardMat, glm::vec3(scale, scale, scale));

    model->render(shaderProgramId, forwardMat);
}

void MarkerModule::add(Pose& pose) {

    modelPoses.push_back(&pose);
}

void MarkerModule::render(GLFWwindow* window, GLuint shaderProgramId, Camera& camera) {

    GLuint lightingLocation =
        glGetUniformLocation(shaderProgramId, "lighting");
    glUniform1i(lightingLocation, false);

    glm::vec3 cameraPosition = camera.getPosition();

    updateMouseState(window);

    updateSelectionState(camera);

    if (hasSelection()) {
        renderModifiers(shaderProgramId, cameraPosition);
        updateModifiers(camera);
    }

    if (!mouse.handled) {
        selectedModelPose = 0;
    }

    renderMarkers(shaderProgramId, cameraPosition);

    glUniform1i(lightingLocation, true);

    modelPoses.clear();
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

    glm::vec3 cameraPosition = camera.getPosition();
    glm::vec3 normal = cameraPosition - mod.dragStartModelPosition;
    glm::vec3 right;

    // TODO: handle width and height parameters properly
    glm::vec3 clickRay =
        camera.pickRay(mouse.x, mouse.y, 800, 600);

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
    }

    glm::vec3 localPoint = intersectionPointInPlaneCoord(
            clickRay,
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
    }

    glm::vec3 cameraPosition = camera.getPosition();

    // TODO: handle width and height parameters properly
    glm::vec3 clickRay =
        camera.pickRay(mouse.x, mouse.y, 800, 600);

    glm::vec3 localPoint = intersectionPointInPlaneCoord(
            clickRay,
            cameraPosition,
            normal,
            mod.dragStartModelPosition,
            right);

    return localPoint;
}
