#include "MarkerModule.h"

MarkerModule::MarkerModule() { 

    markerModel = std::make_shared<Model>("models/marker.obj");
    markerModel->upload();
}

void MarkerModule::addMarker(glm::mat4& model) {

    modelMatrices.push_back(&model);
}

void MarkerModule::removeMarker(glm::mat4& model) {

    modelMatrices.erase(
        std::remove(modelMatrices.begin(), modelMatrices.end(), &model),
        modelMatrices.end());
}

void MarkerModule::render(GLFWwindow* window, GLuint shaderProgramId, Camera& camera) {

    GLuint billboardLocation = glGetUniformLocation(shaderProgramId, "billboard");
    glUniform1i(billboardLocation, true);

    double x, y;
    glfwGetCursorPos(window, &x, &y);

    // TODO: handle width and height parameters properly
    glm::vec3 ray = camera.pickRay(x, y, 800, 600);

    // TODO: maybe choose another vector than the eye vector
    // as the surface normal here
    glm::vec3 eye = glm::normalize(glm::vec3(
            camera.view[0][2], camera.view[1][2], camera.view[2][2]));

    // TODO: inefficient!
    glm::mat4 iv = glm::inverse(camera.view);
    glm::vec3 cameraPosition =
        glm::vec3(iv * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

    for (glm::mat4* modelPtr : modelMatrices) {

        glm::mat4& model = *modelPtr;

        if (GLFW_PRESS == glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) {

            // TODO: inefficient!
            glm::vec3 modelPosition =
                glm::vec3(model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

            float d = glm::length(modelPosition);
            float t = -(glm::dot(cameraPosition, eye) + d) / glm::dot(eye, ray);
            glm::vec3 intersectionPoint = cameraPosition + ray * t;

            if (glm::length(modelPosition - intersectionPoint) < 0.1f) {
                std::cout << "t: " << t << std::endl;
                std::cout << intersectionPoint.x << " "
                    << intersectionPoint.y << " "
                    << intersectionPoint.z << std::endl;
            }
        }

        markerModel->render(shaderProgramId, model);
    }

    glUniform1i(billboardLocation, false);
}
