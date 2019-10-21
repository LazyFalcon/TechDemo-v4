#include "core.hpp"
#include "input-user-pointer.hpp"
#include <GLFW/glfw3.h>
#include "Window.hpp"

namespace
{
const glm::vec2 fullHD {1920.f, 1080.f};
}

InputUserPointer::InputUserPointer(Window& window, glm::vec2 screenSize) : m_window(window), m_screenSize(screenSize) {
    double xpos, ypos;
    glfwGetCursorPos(m_window.window, &xpos, &ypos);

    if(glfwRawMouseMotionSupported()) glfwSetInputMode(m_window.window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    m_systemPointerPosition = glm::vec2(xpos, screenSize.y - ypos);
    m_lastPosition = m_systemPointerPosition;
}

void InputUserPointer::update(float dt) {
    double x, y;
    glfwGetCursorPos(m_window.window, &x, &y);

    if(systemMode())
        setSystemPosition({x, m_screenSize.y - y});
    else
        setSystemPosition(m_screenSize / 2.f);
    setDelta({x - m_lastPosition.x, m_lastPosition.y - y});

    m_lastPosition = {x, y};
    m_velocity = m_positionDelta/dt;
}

glm::vec2 InputUserPointer::screenScale() const {
    return (1.f/m_screenSize) * (m_screenSize/fullHD);
}

void InputUserPointer::hideSystemCursor() {
    glfwSetInputMode(m_window.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    m_useSystemPointer = false;
}
void InputUserPointer::showSystemCursor() {
    glfwSetInputMode(m_window.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    m_useSystemPointer = true;
}
