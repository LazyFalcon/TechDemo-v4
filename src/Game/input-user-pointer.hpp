#pragma once

/*
* Ponieważ pointer jest jeden, jego stan musi być sharowany pomiędzy użytkownikami

*/
class Window;
class InputUserPointer
{
private:
    Window* window;
    glm::vec2 pointerTruePosition {}; // floating, subpixel can be over screen
    glm::vec2 screenSize;
    glm::vec2 pointerOnScreenPosition; // pixelPerfect, clamped to screen position

    bool wrapPosition {true};

    glm::vec2 wrap(){

    }
    glm::vec2 align(glm::vec2 position)
public:

    void hideCursor(){
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    void showCursor(){
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    glm::vec2 getToDraw() const {
        return wrap(align(pointerTruePosition));
    }

    // need to be call each frame to update pointer position, because we have to operate with multiple cameras and pointer settings are derived from camera setings
    // to update pointer position
    void centered(glm::vec2 position(0.5)){
        pointerTruePosition = screenSize*position;
        pointerOnScreenPosition = wrap(align(pointerTruePosition));
    }
    void setFromWorldPosition(const glm::vec4& worldPosition, const glm::mat4& viewMatrix){
        align();
    }
    void moveBy(glm::vec2 pixels){
        pointerTruePosition += pixels;
        pointerOnScreenPosition = wrap(align(pointerTruePosition));
    }
    void hide(){
        wrapPosition = false;
    }
    void show(){
        wrapPosition = true;
        centered();
    }

};