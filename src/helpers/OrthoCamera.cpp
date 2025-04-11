#include "OrthoCamera.h"

glm::mat4 OrthoCamera::getProjectionMatrix() {
    float ver = pose.position.y;
    float hor = aspectRatio * ver;
    return glm::ortho(-hor, hor, -ver, ver, 0.1f, 100.0f);
}

OrthoCamera::OrthoCamera() {
}

OrthoCamera::OrthoCamera(float fov, float aspectRatio)
        : OrthoCamera({0, 0, 0}, 0, fov, aspectRatio) {
}

OrthoCamera::OrthoCamera(glm::vec3 position, float yaw, float fov, float aspectRatio)
        : Camera(fov, aspectRatio), yaw{yaw} {
    prevMouseX = -1;

    pose.position = position;
}

void OrthoCamera::update(GLFWwindow* window, float dt) {

    glm::mat4 view = pose.getInverseMatrix();

    glm::vec3 eye = glm::normalize(
            glm::vec3(view[0][1], 0.0f, view[2][1]));

    glm::vec3 right = glm::normalize(
            glm::vec3(view[0][0], 0.0f, view[2][0]));

    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    float speed = 8 * dt;

    if (GLFW_PRESS == getKey(GLFW_KEY_LEFT_CONTROL)) {
        speed *= 4;
    }

    if (GLFW_PRESS == getKey(GLFW_KEY_LEFT_ALT)) {
        speed /= 4;
    }

    if (GLFW_PRESS == getKey(GLFW_KEY_W)) {
        pose.position += eye * speed;
    }
    if (GLFW_PRESS == getKey(GLFW_KEY_A)) {
        pose.position -= right * speed;
    }
    if (GLFW_PRESS == getKey(GLFW_KEY_S)) {
        pose.position -= eye * speed;
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

        if (prevMouseX > 0) {
            float old_yaw = yaw;

            yaw += std::asin(((float)mouseX - prevMouseX) * dt);

            if (std::isnan(yaw)) {
                yaw = old_yaw;
            }
        }

        prevMouseX = (float)mouseX;
    } else {
        prevMouseX = -1.0f;
    }

    pose.rotation = glm::quat(1, 0, 0, 0);

    pose.rotation = glm::rotate(
            pose.rotation, -yaw, glm::vec3(0.0f, 1.0f, 0.0f));
    pose.rotation = glm::rotate(
            pose.rotation, -M_PI_2f, glm::vec3(1.0f, 0.0f, 0.0f));
}
