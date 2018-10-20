#include "FpsCamera.h"

FpsCamera::FpsCamera() {
}

FpsCamera::FpsCamera(float fov, float aspectRatio) : Camera(fov, aspectRatio) {

    prevMouseX = -1;
    prevMouseY = -1;

    pitch = 0.0f;
    yaw = 0.0f;
}

void FpsCamera::update(GLFWwindow* window, float dt) {

    glm::mat4 view = pose.getInverseMatrix();

    glm::vec3 eye = glm::normalize(
        glm::vec3(view[0][2], 0.0f, view[2][2]));

    glm::vec3 right = glm::normalize(
        glm::vec3(view[0][0], 0.0f, view[2][0]));

    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    float speed = 0.002f * dt;

    if (GLFW_PRESS == getKey(GLFW_KEY_LEFT_CONTROL)) {
        speed *= 4;
    }

    if (GLFW_PRESS == getKey(GLFW_KEY_LEFT_ALT)) {
        speed /= 4;
    }

    if (GLFW_PRESS == getKey(GLFW_KEY_W)) {
        pose.position -= eye * speed;
    }
    if (GLFW_PRESS == getKey(GLFW_KEY_A)) {
        pose.position -= right * speed;
    }
    if (GLFW_PRESS == getKey(GLFW_KEY_S)) {
        pose.position += eye * speed;
    }
    if (GLFW_PRESS == getKey(GLFW_KEY_D)) {
        pose.position += right * speed;
    }
    if (GLFW_PRESS == getKey(GLFW_KEY_SPACE)) {
        pose.position += up * speed;
    }
    if (GLFW_PRESS == getKey(GLFW_KEY_LEFT_SHIFT)) {
        pose.position -= up * speed;
    }
    
    int rightMouseBtnState = getMouseButton(GLFW_MOUSE_BUTTON_RIGHT);

    if (GLFW_PRESS == rightMouseBtnState) {
        double mouseX;
        double mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        if (prevMouseX > 0 && prevMouseY > 0) {
            pitch = std::min(1.56, std::max(-1.56,
                pitch + std::asin((mouseY - prevMouseY) / 200.0f * dt)));
            yaw += std::asin((mouseX - prevMouseX) / 200.0f * dt);
        }

        prevMouseX = mouseX;
        prevMouseY = mouseY;
    } else {
        prevMouseX = -1.0f;
        prevMouseY = -1.0f;
    }

    pose.rotation = glm::quat(1, 0, 0, 0);

    pose.rotation = glm::rotate(
        pose.rotation, -yaw, glm::vec3(0.0f, 1.0f, 0.0f));
    pose.rotation = glm::rotate(
        pose.rotation, -pitch, glm::vec3(1.0f, 0.0f, 0.0f));
}
