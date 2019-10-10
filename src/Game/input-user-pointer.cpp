#include "core.hpp"
#include "input-user-pointer.hpp"
#include <GLFW/glfw3.h>
#include "Window.hpp"

InputUserPointer::InputUserPointer(Window& window, glm::vec2 screenSize) : m_window(window), m_screenSize(screenSize) {
    double xpos, ypos;
    glfwGetCursorPos(window.window, &xpos, &ypos);

    m_systemPointerPosition = glm::vec2(xpos, screenSize.y - ypos);

    showSystemCursor();
}

void InputUserPointer::hideSystemCursor() {
    glfwSetInputMode(m_window.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    m_useSystemPointer = false;
}
void InputUserPointer::showSystemCursor() {
    glfwSetInputMode(m_window.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    m_useSystemPointer = true;
}
