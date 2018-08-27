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
            modelMatrices.begin(), modelMatrices.end(), selectedModelMatrix);

    return it != modelMatrices.end();
}

float MarkerModule::getScale(glm::vec3& cameraPosition, glm::vec3& modelPosition) {
    
    return glm::length(cameraPosition - modelPosition) * 0.05;
}

glm::vec3 MarkerModule::getModelPosition(glm::mat4* modelPtr) {

    glm::mat4& modelMat = *modelPtr;
    return glm::vec3(modelMat[3][0], modelMat[3][1], modelMat[3][2]);
}

void MarkerModule::updateMouseState(GLFWwindow* window) {

    double mouseX;
    double mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    mouse.x = (float)mouseX;
    mouse.y = (float)mouseY;

    static int prevButtonState = GLFW_RELEASE;
    int buttonState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);

    mouse.pressed = GLFW_PRESS == buttonState;
    mouse.click = GLFW_RELEASE == prevButtonState && GLFW_PRESS == buttonState;

    if (mouse.click) {
        mouse.handled = false;
    }

    prevButtonState = buttonState;
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

    for (glm::mat4* modelPtr : modelMatrices) {

        glm::vec3 modelPosition = getModelPosition(modelPtr);

        float scale = getScale(cameraPosition, modelPosition);

        glm::vec3 intersectionPoint = intersectLineWithPlane(
                clickRay, cameraPosition, eyeVector, modelPosition);

        if (glm::length(modelPosition - intersectionPoint) < 0.5f * scale) {

            if (selectedModelMatrix == modelPtr) {
                selectionMode = (SelectionMode)((selectionMode + 1) % 3);
            } else {
                selectionMode = TRANSLATE;
            }

            mouse.handled = true;
            selectedModelMatrix = modelPtr;
        }
    }

    if (!mouse.handled) {
        selectedModelMatrix = 0;
    }
}

void MarkerModule::updateModifiers(Camera& camera) {

    if (!mouse.pressed) {
        return;
    }

    glm::mat4& modelMat = *selectedModelMatrix;
    glm::vec3 cameraPosition = camera.getPosition();

    if (mouse.click) {
        mod.dragStartModelPosition = getModelPosition(selectedModelMatrix);
        mod.dragStartScale = getScale(cameraPosition, mod.dragStartModelPosition);

        float arrowLength = 4.5f * mod.dragStartScale;
        float arrowHeight = 0.5f * mod.dragStartScale;

        if (selectionMode == TRANSLATE || selectionMode == SCALE) {
            glm::vec3 xLocalPoint = mousePosInArrowCoords(camera, X_AXIS);
            glm::vec3 yLocalPoint = mousePosInArrowCoords(camera, Y_AXIS);
            glm::vec3 zLocalPoint = mousePosInArrowCoords(camera, Z_AXIS);

            mouse.handled = true;

            if (xLocalPoint.x < arrowLength && xLocalPoint.x > 0
                    && std::abs(xLocalPoint.y) < arrowHeight) {
                selectedAxis = X_AXIS;
            } else if (yLocalPoint.x < arrowLength && yLocalPoint.x > 0
                    && std::abs(yLocalPoint.y) < arrowHeight) {
                selectedAxis = Y_AXIS;
            } else if (zLocalPoint.x < arrowLength && zLocalPoint.x > 0
                    && std::abs(zLocalPoint.y) < arrowHeight) {
                selectedAxis = Z_AXIS;
            } else {
                mouse.handled = false;
                selectedAxis = NONE;
            }

            mod.prevDragState.x = selectedAxis == X_AXIS ? xLocalPoint.x : 0;
            mod.prevDragState.y = selectedAxis == Y_AXIS ? yLocalPoint.x : 0;
            mod.prevDragState.z = selectedAxis == Z_AXIS ? zLocalPoint.x : 0;
        } else {
            // TODO
        }
    } else {
        glm::mat4 translationMat;
        glm::mat4 scaleMat;
        glm::mat4 rotationMat;

        decomposeTransformationMatrix(
                modelMat,
                translationMat,
                scaleMat,
                rotationMat);

        glm::vec3 dragState;

        if (selectionMode == TRANSLATE || selectionMode == SCALE) {
            dragState.x = selectedAxis == X_AXIS ?
                mousePosInArrowCoords(camera, X_AXIS).x : 0;
            dragState.y = selectedAxis == Y_AXIS ?
                mousePosInArrowCoords(camera, Y_AXIS).x : 0;
            dragState.z = selectedAxis == Z_AXIS ?
                mousePosInArrowCoords(camera, Z_AXIS).x : 0;
        } else {
            // TODO
        }

        switch(selectionMode) {
            case TRANSLATE: {
                glm::vec3 translation = dragState - mod.prevDragState;
                translationMat = glm::translate(translationMat, translation);
                break;
            }
            case SCALE: {
                glm::vec3 scale = dragState - mod.prevDragState;
                scale.x = std::tanh(scale.x);
                scale.y = std::tanh(scale.y);
                scale.z = std::tanh(scale.z);
                scale += glm::vec3(1.0f, 1.0f, 1.0f);
                scaleMat = glm::scale(scaleMat, scale);
                break;
            }
            case ROTATE: {
                break;
            }
        }

        std::cout << scaleMat << std::endl;

        mod.prevDragState = dragState;

        modelMat = translationMat * rotationMat * scaleMat;
    }
}

void MarkerModule::renderMarkers(GLuint shaderProgramId, glm::vec3& cameraPosition) {

    GLuint billboardLocation =
        glGetUniformLocation(shaderProgramId, "billboard");
    glUniform1i(billboardLocation, true);

    for (glm::mat4* modelPtr : modelMatrices) {

        glm::vec3 modelPosition = getModelPosition(modelPtr);

        float scale = getScale(cameraPosition, modelPosition);

        glm::mat4 markerMat = glm::mat4(1.0f);
        markerMat = glm::translate(markerMat, modelPosition);
        markerMat = glm::scale(markerMat, glm::vec3(scale, scale, scale));

        if (selectedModelMatrix == modelPtr) {
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
    
    glm::vec3 modelPosition = getModelPosition(selectedModelMatrix);

    float scale = getScale(cameraPosition, modelPosition);

    switch(selectionMode) {
        case TRANSLATE:
            renderGlyphTriplet(
                    shaderProgramId,
                    arrowModel,
                    modelPosition, 
                    scale,
                    1.0f);
            break;
        case SCALE: 
            renderGlyphTriplet(
                    shaderProgramId,
                    scaleArrowModel,
                    modelPosition,
                    scale,
                    1.0f);
            break;
        case ROTATE:
            renderGlyphTriplet(
                    shaderProgramId,
                    ringModel,
                    modelPosition,
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

void MarkerModule::addMarker(glm::mat4& model) {

    modelMatrices.push_back(&model);
}

void MarkerModule::render(GLFWwindow* window, GLuint shaderProgramId, Camera& camera) {

    GLuint lightingLocation =
        glGetUniformLocation(shaderProgramId, "lighting");
    glUniform1i(lightingLocation, false);

    glm::vec3 cameraPosition = camera.getPosition();

    updateMouseState(window);

    if (hasSelection()) {
        renderModifiers(shaderProgramId, cameraPosition);
        updateModifiers(camera);
    }

    updateSelectionState(camera);

    renderMarkers(shaderProgramId, cameraPosition);

    // TODO: handle width and height parameters properly
    /*
    glm::vec3 clickRay =
        camera.pickRay(mouse.x, mouse.y, 800, 600);

    for (glm::mat4* modelPtr : modelMatrices) {

        glm::mat4& modelMat = *modelPtr;
        glm::vec3 modelPosition = getModelPosition(modelPtr);

        float scale = getScale(cameraPosition, modelPosition);

        if (selectedModelMatrix == modelPtr) {

            glm::mat4 translationMat;
            glm::mat4 scaleMat;
            glm::mat4 rotationMat;

            decomposeTransformationMatrix(
                    modelMat,
                    translationMat,
                    scaleMat,
                    rotationMat);

            switch(selectionMode) {
                case TRANSLATE: {
                    glm::vec3 shift(0.0f, 0.0f, 0.0f);
                    calcShift(
                            shift, 
                            clickRay,
                            cameraPosition,
                            modelPosition,
                            scale);

                    if (!mouse.click) {
                        shift -= prevShift;
                    }

                    //translationMat = glm::translate(translationMat, shift);
                    break;
                }
                case SCALE: {
                    glm::vec3 shift;
                    calcShift(
                            shift, 
                            clickRay,
                            cameraPosition,
                            modelPosition,
                            scale);

                    shift = glm::abs(shift) + glm::vec3(1.0f, 1.0f, 1.0f);

                    scaleMat = glm::scale(scaleMat, shift);
                    break;
                }
                case ROTATE:
                    break;
            }

            modelMat = translationMat * rotationMat * scaleMat;
        }
    }
    */

    glUniform1i(lightingLocation, true);

    modelMatrices.clear();
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
            normal = glm::vec3(1.0f, 0.0f, 0.0f);
            right = glm::vec3(0.0f, 0.0f, 1.0f);
            break;
        case Y_AXIS:
            normal = glm::vec3(0.0f, 1.0f, 0.0f);
            right = glm::vec3(1.0f, 0.0f, 0.0f);
            break;
        case Z_AXIS:
            normal = glm::vec3(0.0f, 0.0f, 0.1f);
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

void MarkerModule::decomposeTransformationMatrix(
        glm::mat4& matrix,
        glm::mat4& translationMatrix,
        glm::mat4& scaleMatrix,
        glm::mat4& rotationMatrix) {

    translationMatrix = glm::mat4(1.0f);
    translationMatrix[3][0] = matrix[3][0];
    translationMatrix[3][1] = matrix[3][1];
    translationMatrix[3][2] = matrix[3][2];

    scaleMatrix = glm::mat4(1.0f);
    scaleMatrix[0][0] = glm::length(matrix[0]);
    scaleMatrix[1][1] = glm::length(matrix[1]);
    scaleMatrix[2][2] = glm::length(matrix[2]);

    rotationMatrix = glm::mat4(1.0f);
    rotationMatrix[0] = matrix[0] / scaleMatrix[0][0];
    rotationMatrix[1] = matrix[1] / scaleMatrix[1][1];
    rotationMatrix[2] = matrix[2] / scaleMatrix[2][2];
}
