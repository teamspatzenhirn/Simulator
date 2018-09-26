#include "FpsCamera.h"

FpsCamera::FpsCamera(float fov, float aspectRatio) : Camera(fov, aspectRatio) {

    translation = glm::mat4(1.0f);

    prevMouseX = -1;
    prevMouseY = -1;

    pitch = 0.0f;
    yaw = 0.0f;
}

void FpsCamera::update(GLFWwindow* window, float dt) {

    glm::vec3 eye = glm::normalize(
        glm::vec3(view[0][2], 0.0f, view[2][2]));

    glm::vec3 right = glm::normalize(
        glm::vec3(view[0][0], 0.0f, view[2][0]));

    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    float speed = 0.01f * dt;

    if (GLFW_PRESS == getKey(GLFW_KEY_W)) {
        translation = glm::translate(translation, eye * speed);
    }
    if (GLFW_PRESS == getKey(GLFW_KEY_A)) {
        translation = glm::translate(translation, right * speed);
    }
    if (GLFW_PRESS == getKey(GLFW_KEY_S)) {
        translation = glm::translate(translation, eye * -speed);
    }
    if (GLFW_PRESS == getKey(GLFW_KEY_D)) {
        translation = glm::translate(translation, right * -speed);
    }
    if (GLFW_PRESS == getKey(GLFW_KEY_SPACE)) {
        translation = glm::translate(translation, up * -speed);
    }
    if (GLFW_PRESS == getKey(GLFW_KEY_LEFT_SHIFT)) {
        translation = glm::translate(translation, up * speed);
    }
    
    int rightMouseBtnState = getMouseButton(GLFW_MOUSE_BUTTON_RIGHT);

    if (GLFW_PRESS == rightMouseBtnState) {
        double mouseX;
        double mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        if (prevMouseX > 0 && prevMouseY > 0) {
            pitch = std::min(1.56, std::max(-1.56,
                pitch + std::asin((mouseY - prevMouseY) / 10000.0f * dt)));
            yaw += std::asin((mouseX - prevMouseX) / 10000.0f * dt);
        }

        prevMouseX = mouseX;
        prevMouseY = mouseY;
    } else {
        prevMouseX = -1.0f;
        prevMouseY = -1.0f;
    }

    glm::mat4 rotateX = glm::rotate(
        glm::mat4(1.0f), pitch, glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 rotateY = glm::rotate(
        glm::mat4(1.0f), yaw, glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 rotation = rotateX * rotateY;

    view = rotation * translation;
}
