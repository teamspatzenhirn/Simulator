#include "Input.h"

int lastKeyState[400];
double cursorPosX = 0;
double cursorPosY = 0;
int lastMouseButtonState[8];

std::vector<KeyEvent> keyEvents;
std::vector<CharEvent> charEvents;
std::vector<CharModsEvent> charModsEvents;
std::vector<MouseButtonEvent> mouseButtonEvents;
std::vector<CursorPosEvent> cursorPosEvents;
std::vector<CursorEnterEvent> cursorEnterEvents;
std::vector<ScrollEvent> scrollEvents;
std::vector<DropEvent> dropEvents;

void keyEventsCallback(
        GLFWwindow* window, int key, int scancode, int action, int mods) {

    if (action != GLFW_REPEAT) {
        lastKeyState[key] = action;
    }
    keyEvents.push_back({window, key, scancode, action, mods});
}

void charEventsCallback(
        GLFWwindow* window, unsigned int codepoint) {

    charEvents.push_back({window, codepoint});
}

void charModsEventsCallback(
        GLFWwindow* window, unsigned int codepoint, int mods) {

    charModsEvents.push_back({window, codepoint, mods});
}

void mouseButtonEventsCallback(
        GLFWwindow* window, int button, int action, int mods) {

    lastMouseButtonState[button] = action;

    mouseButtonEvents.push_back({window, button, action, mods});
}

void cursorPosEventsCallback(
        GLFWwindow* window, double xpos, double ypos) {

    cursorPosX = xpos;
    cursorPosY = ypos;

    cursorPosEvents.push_back({window, xpos, ypos});
}

void cursorEnterEventsCallback(
        GLFWwindow* window, int entered) {

    cursorEnterEvents.push_back({window, entered});
}

void scrollEventsCallback(
        GLFWwindow* window, double xoffset, double yoffset) {

    scrollEvents.push_back({window, xoffset, yoffset});
}

void dropEventsCallback(
        GLFWwindow* window, int count, const char** paths) {

    dropEvents.push_back({window, count, paths});
}

void initInput(GLFWwindow* window) {

    for (int i = 0; i < 400; i++) {
        lastKeyState[i] = glfwGetKey(window, i);
    }

    glfwGetCursorPos(window, &cursorPosX, &cursorPosY);

    for (int i = 0; i < 8; i++) {
        lastMouseButtonState[i] = glfwGetMouseButton(window, i);
    }

    glfwSetKeyCallback(window, keyEventsCallback);
    glfwSetCharCallback(window, charEventsCallback);
    glfwSetCharModsCallback(window, charModsEventsCallback);
    glfwSetMouseButtonCallback(window, mouseButtonEventsCallback);
    glfwSetCursorPosCallback(window, cursorPosEventsCallback);
    glfwSetCursorEnterCallback(window, cursorEnterEventsCallback);
    glfwSetScrollCallback(window, scrollEventsCallback);
    glfwSetDropCallback(window, dropEventsCallback);
}

void updateInput() {

    keyEvents.clear();
    charEvents.clear();
    charModsEvents.clear();
    mouseButtonEvents.clear();
    cursorPosEvents.clear();
    cursorEnterEvents.clear();
    scrollEvents.clear();
    dropEvents.clear();

    glfwPollEvents();
}

void clearKeyboardInput() {

    for (int i = 0; i < 400; ++i) {
        lastKeyState[i] = GLFW_RELEASE;
    }

    keyEvents.clear();
    charEvents.clear();
    charModsEvents.clear();
}

void clearMouseInput() {

    for (int i = 0; i < 8; ++i) {
        lastMouseButtonState[i] = GLFW_RELEASE;
    }

    mouseButtonEvents.clear();
    cursorPosEvents.clear();
    cursorEnterEvents.clear();
    scrollEvents.clear();
    dropEvents.clear();
}

int getKey(int key) {
    return lastKeyState[key];
}

double getCursorX() {
    return cursorPosX;
}

double getCursorY() {
    return cursorPosY;
}

int getMouseButton(int button) {
    return lastMouseButtonState[button];
}

std::vector<KeyEvent>& getKeyEvents() {
    return keyEvents;
}

std::vector<CharEvent>& getCharEvents() {
    return charEvents;
}

std::vector<CharModsEvent>& getCharModsEvents() {
    return charModsEvents;
}

std::vector<MouseButtonEvent>& getMouseButtonEvents() {
    return mouseButtonEvents;
}

std::vector<CursorPosEvent>& getCursorPosEvents() {
    return cursorPosEvents;
}

std::vector<CursorEnterEvent>& getCursorEnterEvents() {
    return cursorEnterEvents;
}

std::vector<ScrollEvent>& getScrollEvents() {
    return scrollEvents;
}

std::vector<DropEvent>& getDropEvents() { 
    return dropEvents;
}
