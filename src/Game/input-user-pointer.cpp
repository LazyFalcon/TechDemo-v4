#include "core.hpp"
#include <GLFW/glfw3.h>
#include "input-user-pointer.hpp"
#include "Window.hpp"

InputUserPointer::InputUserPointer(Window& window, glm::vec2 screenSize): window(window), screenSize(screenSize){
    double xpos, ypos;
    glfwGetCursorPos(window.window, &xpos, &ypos);

    systemPointerPosition = glm::vec2(xpos, screenSize.y-ypos);


    showSystemCursor();
}

void InputUserPointer::hideSystemCursor(){
    glfwSetInputMode(window.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    useSystemPointer = false;
}
void InputUserPointer::showSystemCursor(){
    glfwSetInputMode(window.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    useSystemPointer = true;
}
