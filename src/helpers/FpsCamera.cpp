#include "FpsCamera.h"

FpsCamera::FpsCamera() {
}

FpsCamera::FpsCamera(float fov, float aspectRatio)
    : FpsCamera({0, 0, 0}, 0, 0, fov, aspectRatio) {

}

FpsCamera::FpsCamera(glm::vec3 position, float pitch, float yaw, float fov, float aspectRatio)
    : Camera(fov, aspectRatio), pitch{pitch}, yaw{yaw} {

    prevMouseX = -1;
    prevMouseY = -1;

    pose.position = position;
}

void FpsCamera::update(GLFWwindow* window, float dt) {

    glm::mat4 view = pose.getInverseMatrix();

    glm::vec3 eye = glm::normalize(
        glm::vec3(view[0][2], 0.0f, view[2][2]));

    glm::vec3 right = glm::normalize(
        glm::vec3(view[0][0], 0.0f, view[2][0]));

    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    float speed = 2 * dt;

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

            pitch += std::asin(((float)mouseY - prevMouseY) / 700.0f * dt);
            pitch = std::min(1.56f, std::max(-1.56f, pitch));

            yaw += std::asin(((float)mouseX - prevMouseX) / 700.0f * dt);
        }

        prevMouseX = (float)mouseX;
        prevMouseY = (float)mouseY;
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
