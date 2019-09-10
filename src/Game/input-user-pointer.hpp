#pragma once

/*
* Ponieważ pointer jest jeden, jego stan musi być sharowany pomiędzy użytkownikami

*/
class Window;
class InputUserPointer
{
private:
    Window* window;
    glm::vec2 gamePointerPosition {}; // floating, subpixel can be over screen
    glm::vec2 screenSize;
    glm::vec2 pointerOnScreenPosition; // pixelPerfect, clamped to screen position

    glm::vec2 systemPointerPosition{};
    glm::vec2 lastFrameShift{};

    bool wrapPosition {true};
    bool useSystemPointer {false};
    bool hideGamePointer {false};

    glm::vec2 wrap(glm::vec2 in){
        return in;
    }
    // when, in future, pointer will have subpixel precision
    glm::vec2 align(glm::vec2 in){
        return in;
    }

public:
    InputUserPointer(glm::vec2 screenSize): screenSize(screenSize){
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        systemPointerPosition = glm::vec2(xpos, screenSize.y-ypos);


        showSystemCursor();
    }

    void hideSystemCursor(){
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        useSystemPointer = false;
    }
    void showSystemCursor(){
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        useSystemPointer = true;
    }
    void hide(){
        hideGamePointer = true;
    }
    void show(){
        hideGamePointer = false;
        centered();
    }


    bool visible() const {
        return not useSystemPointer and not hideGamePointer;
    }
    glm::vec2 screenPxPosition() const {
        if(useSystemPointer) return systemPointerPosition;
        return wrap(align(gamePointerPosition));
    }
    glm::vec2 screenPosition() const {
        if(useSystemPointer) return systemPointerPosition/screenSize;
        return wrap(align(gamePointerPosition))/screenSize;
    }
    glm::vec2 pxMovement() const {
        return lastFrameShift * screenSize;
    }
    glm::vec2 movement() const {
        return lastFrameShift;
    }

    void setSystemPosition(glm::vec2 position){
        systemPointerPosition = position;
    }

    void moveBy(glm::vec2 relativeShift){
        lastFrameShift = relativeShift;
        if(useSystemPointer) return;
        gamePointerPosition += relativeShift*screenSize;
    }

    // to be called each frame to update in-game pointer position
    void centered(glm::vec2 position = glm::vec2(0.5f)){
        gamePointerPosition = screenSize*position;
        pointerOnScreenPosition = wrap(align(gamePointerPosition));
    }
    void setFromWorldPosition(const glm::vec4& worldPosition, const glm::mat4& viewMatrix){
        gamePointerPosition = glm::project(worldPosition.xyz(), glm::mat4(1), viewMatrix, glm::vec4(0,0,screensize));
        pointerOnScreenPosition = wrap(align(gamePointerPosition));
    }

};