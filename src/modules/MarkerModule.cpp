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

    isClick = true;
}

void MarkerModule::addMarker(glm::mat4& model) {

    modelMatrices.push_back(&model);
}

void MarkerModule::render(GLFWwindow* window, GLuint shaderProgramId, Camera& camera) {

    GLuint billboardLocation =
        glGetUniformLocation(shaderProgramId, "billboard");
    glUniform1i(billboardLocation, true);

    GLuint lightingLocation =
        glGetUniformLocation(shaderProgramId, "lighting");
    glUniform1i(lightingLocation, false);

    glm::vec2 mousePosition;

    /* load mouse position */ {
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        mousePosition.x = mouseX;
        mousePosition.y = mouseY;
    }

    // TODO: handle width and height parameters properly
    glm::vec3 ray = camera.pickRay(mousePosition.x, mousePosition.y, 800, 600);

    // TODO: maybe choose another vector than the eye vector
    // as the intersection surface normal here
    glm::vec3 eye = glm::normalize(glm::vec3(
            camera.view[0][2], camera.view[1][2], camera.view[2][2]));

    // TODO: inefficient!
    glm::mat4 iv = glm::inverse(camera.view);
    glm::vec3 cameraPosition =
        glm::vec3(iv * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

    bool handled = false;

    for (glm::mat4* modelPtr : modelMatrices) {

        glm::mat4& model = *modelPtr;

        glm::vec3 modelPosition =
            glm::vec3(model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

        float scale = glm::length(cameraPosition - modelPosition) / 20;

        if (GLFW_PRESS == glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) {

            glm::vec3 intersectionPoint = intersectLineWithPlane(
                    ray, cameraPosition, eye, modelPosition);

            /*
            std::cout << "Intersection:" << std::endl;
            std::cout << intersectionPoint.x
                      << " " << intersectionPoint.y
                      << " " << intersectionPoint.z
                      << std::endl;

            std::cout << "Model:" << std::endl;
            std::cout << modelPosition.x
                      << " " << modelPosition.y
                      << " " << modelPosition.z
                      << std::endl;
            */

            if (glm::length(modelPosition - intersectionPoint) < 0.5f * scale) {
                if (selectedMarker != modelPtr) {
                    selectedMarker = modelPtr;
                    selectionMode = TRANSLATE;
                } else if (isClick) {
                    selectionMode = (SelectionMode)((selectionMode + 1) % 3);
                }
                handled = true;
            }
        }

        glm::mat4 markerMat = glm::mat4(1.0f);
        markerMat = glm::translate(markerMat, modelPosition);
        markerMat = glm::scale(markerMat, glm::vec3(scale, scale, scale));

        if (selectedMarker == modelPtr) {

            glUniform1i(billboardLocation, false);

            switch(selectionMode) {
                case TRANSLATE: {
                    glm::vec3 shift(0.0f, 0.0f, 0.0f);
                    handled = updateTranslation(
                            shift, 
                            window,
                            ray,
                            cameraPosition,
                            modelPosition,
                            scale);

                    model[3][0] += shift.x;
                    model[3][1] += shift.y;
                    model[3][2] += shift.z;
                    
                    std::cout << shift.x << " " << shift.y << " " << shift.z << std::endl;
                    renderModifiers(
                            shaderProgramId,
                            arrowModel,
                            modelPosition, 
                            scale,
                            1.0f);
                    break;
                }
                case SCALE:
                    renderModifiers(
                            shaderProgramId,
                            scaleArrowModel,
                            modelPosition,
                            scale,
                            1.0f);
                    break;
                case ROTATE:
                    renderModifiers(
                            shaderProgramId,
                            ringModel,
                            modelPosition,
                            scale * 3,
                            0.0f);
                    break;
            }

            glUniform1i(billboardLocation, true);

            markerModel->material.Ka = objl::Vector3(1.0f, 1.0f, 0.0f);
            markerModel->material.Kd = objl::Vector3(1.0f, 1.0f, 0.0f);
            markerModel->material.Ks = objl::Vector3(0.0f, 0.0f, 0.0f);
            markerModel->render(shaderProgramId, markerMat);
        } else {
            markerModel->material.Ka = objl::Vector3(0.0f, 1.0f, 0.0f);
            markerModel->material.Kd = objl::Vector3(0.0f, 1.0f, 0.0f);
            markerModel->material.Ks = objl::Vector3(0.0f, 1.0f, 0.0f);
            markerModel->render(shaderProgramId, markerMat);
        }
    }

    glUniform1i(billboardLocation, false);
    glUniform1i(lightingLocation, true);

    if (GLFW_PRESS == glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) {
        if (isClick && !handled) {
            selectedMarker = 0;
        }
        isClick = false;
    } else if (GLFW_RELEASE == glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) {
        isClick = true;
    }

    modelMatrices.clear();
}

void MarkerModule::renderModifiers(
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

bool MarkerModule::updateTranslation(
        glm::vec3& shift, 
        GLFWwindow* window,
        glm::vec3& clickRay,
        glm::vec3& cameraPosition,
        glm::vec3& modelPosition,
        float scale) {

    if (GLFW_PRESS == glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) {

        bool handled = false;

        glm::vec3 normal = modelPosition - cameraPosition;

        float offset = 2.5f * scale;
        float width = 1.75f * scale;
        float height = 0.5f * scale;

        glm::vec3 xNormal = glm::normalize(glm::vec3(0.0f, normal.y, normal.z));
        glm::vec3 xRight = glm::vec3(1.0f, 0.0f, 0.0f);
        glm::vec3 yNormal = glm::normalize(glm::vec3(normal.x, 0.0f, normal.z));
        glm::vec3 yRight = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 zNormal = glm::normalize(glm::vec3(normal.x, normal.y, 0.0f));
        glm::vec3 zRight = glm::vec3(0.0f, 0.0f, 1.0f);

        glm::vec3 xLocalPoint = intersectionPointInPlaneCoord(
                clickRay,
                cameraPosition,
                xNormal,
                modelPosition,
                xRight);

        std::cout << "xLocalPoint:" << std::endl;
        std::cout << xLocalPoint.x
                  << " " << xLocalPoint.y
                  << " " << xLocalPoint.z
                  << std::endl;

        std::cout << width << " " << height << std::endl;

        glm::vec3 yLocalPoint = intersectionPointInPlaneCoord(
                clickRay,
                cameraPosition,
                yNormal,
                modelPosition,
                yRight);

        glm::vec3 zLocalPoint = intersectionPointInPlaneCoord(
                clickRay,
                cameraPosition,
                zNormal,
                modelPosition,
                zRight);

        xLocalPoint -= glm::vec3(offset, 0.0f, 0.0f);
        yLocalPoint -= glm::vec3(offset, 0.0f, 0.0f);
        zLocalPoint -= glm::vec3(offset, 0.0f, 0.0f);

        if (isClick) {
            if (std::abs(xLocalPoint.x) < width && std::abs(xLocalPoint.y) < height) {
                selectedAxis = X_AXIS;
                handled = true;
            }
            if (std::abs(yLocalPoint.x) < width && std::abs(yLocalPoint.y) < height) {
                selectedAxis = Y_AXIS;
                handled = true;
            }
            if (std::abs(zLocalPoint.x) < width && std::abs(zLocalPoint.y) < height) {
                selectedAxis = Z_AXIS;
                handled = true;
            }
        } else {
            switch (selectedAxis) {
                case X_AXIS:
                    shift.x = xLocalPoint.x - mousePrevShift.x;
                    break;
                case Y_AXIS:
                    shift.y = yLocalPoint.x - mousePrevShift.y;
                    break;
                case Z_AXIS:
                    shift.z = zLocalPoint.x - mousePrevShift.z;
                    break;
            } 
        }

        mousePrevShift.x = xLocalPoint.x;
        mousePrevShift.y = yLocalPoint.x;
        mousePrevShift.z = zLocalPoint.x;

        return handled;
    } 
    
    return true;
}
