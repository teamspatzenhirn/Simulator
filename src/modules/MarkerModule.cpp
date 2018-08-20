#include "MarkerModule.h"

MarkerModule::MarkerModule() { 

    markerModel = std::make_shared<Model>("models/marker.obj");
    markerModel->upload();

    markerModelSelected = std::make_shared<Model>("models/marker_selected.obj");
    markerModelSelected->upload();

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

    GLuint billboardLocation = glGetUniformLocation(shaderProgramId, "billboard");
    glUniform1i(billboardLocation, true);

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

    bool removeSelection = true;

    for (glm::mat4* modelPtr : modelMatrices) {

        glm::mat4& model = *modelPtr;

        // TODO: inefficient!
        glm::vec3 modelPosition =
            glm::vec3(model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

        if (GLFW_PRESS == glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) {

            /*
            float d = glm::length(modelPosition);
            float t = -(glm::dot(cameraPosition, eye) + d) / glm::dot(eye, ray);
            glm::vec3 intersectionPoint = cameraPosition + ray * t;
            */

            glm::vec3 intersectionPoint = intersectLineWithPlane(
                    ray, cameraPosition, eye, modelPosition);

            if (glm::length(modelPosition - intersectionPoint) < 0.1f) {
                if (selectedMarker != modelPtr) {
                    selectedMarker = modelPtr;
                    selectionMode = TRANSLATE;
                    removeSelection = false;
                } else if (isClick) {
                    selectionMode = (SelectionMode)((selectionMode + 1) % 3);
                    removeSelection = false;
                }
            }
        }

        if (selectedMarker == modelPtr) {

            glUniform1i(billboardLocation, false);

            switch(selectionMode) {
                case TRANSLATE:
                    removeSelection = updateTranslation(
                            mousePosition,
                            ray,
                            cameraPosition,
                            modelPosition);
                    renderModifiers(shaderProgramId, arrowModel, 0.2f, 0.2f);
                    break;
                case SCALE:
                    renderModifiers(shaderProgramId, scaleArrowModel, 0.2f, 0.2f);
                    break;
                case ROTATE:
                    renderModifiers(shaderProgramId, ringModel, 0.6f, 0.0f);
                    break;
            }

            glUniform1i(billboardLocation, true);

            markerModelSelected->render(shaderProgramId, model);
        } else {
            markerModel->render(shaderProgramId, model);
        }
    }

    glUniform1i(billboardLocation, false);

    if (GLFW_PRESS == glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) {
        if (isClick && removeSelection) {
            selectedMarker = 0;
        }
        isClick = false;
        mousePrev = mousePosition;
    } else if (GLFW_RELEASE == glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) {
        isClick = true;
    }

    modelMatrices.clear();
}

void MarkerModule::renderModifiers(
            GLuint shaderProgramId,
            std::shared_ptr<Model> model,
            float scale, 
            float offset) {

    glm::mat4 upMat = glm::mat4(1.0f);
    upMat = glm::translate(upMat, glm::vec3(0.0f, offset, 0.0f));
    upMat = glm::scale(upMat, glm::vec3(scale, scale, scale));

    model->material.Ka = objl::Vector3(0.0f, 1.0f, 0.0f);
    model->material.Kd = objl::Vector3(0.0f, 1.0f, 0.0f);
    model->material.Ks = objl::Vector3(0.0f, 1.0f, 0.0f);

    model->render(shaderProgramId, upMat);

    model->material.Ka = objl::Vector3(1.0f, 0.0f, 0.0f);
    model->material.Kd = objl::Vector3(1.0f, 0.0f, 0.0f);
    model->material.Ks = objl::Vector3(1.0f, 0.0f, 0.0f);

    glm::mat4 rightMat = glm::mat4(1.0f);
    rightMat = glm::rotate(rightMat,
            -(float)M_PI / 2.0f, glm::vec3(0.0f, 0.0f, 1.0f));
    rightMat = glm::translate(rightMat, glm::vec3(0.0f, offset, 0.0f));
    rightMat = glm::scale(rightMat, glm::vec3(scale, scale, scale));

    model->render(shaderProgramId, rightMat);

    model->material.Ka = objl::Vector3(0.0f, 0.0f, 1.0f);
    model->material.Kd = objl::Vector3(0.0f, 0.0f, 1.0f);
    model->material.Ks = objl::Vector3(0.0f, 0.0f, 1.0f);

    glm::mat4 forwardMat = glm::mat4(1.0f);
    forwardMat = glm::rotate(forwardMat,
            (float)M_PI / 2.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    forwardMat = glm::translate(forwardMat, glm::vec3(0.0f, offset, 0.0f));
    forwardMat = glm::scale(forwardMat, glm::vec3(scale, scale, scale));

    model->render(shaderProgramId, forwardMat);
}

glm::vec3 MarkerModule::intersectLineWithPlane(
        glm::vec3& lineDirection,
        glm::vec3& linePoint,
        glm::vec3& planeNormal,
        glm::vec3& planePoint) {

    float t = -(glm::dot(linePoint, planeNormal) + glm::length(planePoint));
    t /= glm::dot(planeNormal, lineDirection);

    return linePoint + lineDirection * t;
}

bool MarkerModule::updateTranslation(
        glm::vec2& mousePosition,
        glm::vec3& clickRay,
        glm::vec3& cameraPosition,
        glm::vec3& modelPosition) {

    glm::vec3 normal = modelPosition - cameraPosition;

    glm::vec3 xNormal = glm::normalize(glm::vec3(0, normal.y, normal.z));
    glm::vec3 xRight = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 xUp = glm::cross(xNormal, xRight);

    glm::vec3 xIntersectionPoint = intersectLineWithPlane(
            clickRay,
            cameraPosition,
            xNormal,
            modelPosition);

    glm::mat3 xToPlaneCoords =
        glm::transpose(glm::mat3(xRight, xUp, xNormal));

    glm::vec3 xLocalPoint = xToPlaneCoords * xIntersectionPoint;
    xLocalPoint -= modelPosition + glm::vec3(0.5, 0.0f, 0.0f);

    if (std::abs(xLocalPoint.x) < 0.4 && std::abs(xLocalPoint.y) < 0.1) {
        return false;
    }

    glm::vec3 yNormal = glm::normalize(glm::vec3(normal.x, 0, normal.z));
    glm::vec3 yRight = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 yUp = glm::cross(yNormal, yRight);

    glm::vec3 yIntersectionPoint = intersectLineWithPlane(
            clickRay,
            cameraPosition,
            yNormal,
            modelPosition);

    glm::mat3 yToPlaneCoords =
        glm::transpose(glm::mat3(yRight, yUp, yNormal));

    glm::vec3 yLocalPoint = yToPlaneCoords * yIntersectionPoint;
    yLocalPoint -= modelPosition + glm::vec3(0.0f, 0.5f, 0.0f);

    if (std::abs(yLocalPoint.x) < 0.4 && std::abs(yLocalPoint.y) < 0.1) {
        return false;
    }

    return true;

    /*
    glm::vec3 yxPoint = intersectLineWithPlane(
            clickRay,
            cameraPosition,
            glm::vec3(0.0f, 0.0f, 1.0f),
            modelPosition);

    glm::vec3 zyPoint = intersectLineWithPlane(
            clickRay,
            cameraPosition,
            glm::vec3(1.0f, 0.0f, 0.0f),
            modelPosition);

    yxPoint -= modelPosition;
    zyPoint -= modelPosition;
    */
}
