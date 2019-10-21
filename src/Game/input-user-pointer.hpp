#pragma once

/*
Jesli blenderlike-freecam to używamy myszy systemowej i obracamy dookoła punktu, mysz late se swobodnie(przydałoby się ja zapętlić)
Jesli freecam to mysz jest przypięta na środku albo pokazuje wychelenie

Przypięta może być tylko na środku albo pokazywać wychylenie.

Input jest w postaci pozycji systemowej(zarezerwowanej wyłącznie dla kursora)
                     Przesunięcia: raw input myszy, używany do obrotów kamery
                     Odchylenie jest podawane przez usera, wyliczone z kamery
*/
class Window;
class InputUserPointer
{
private:
    enum Mode
    {
        SYSTEM,
        GAME
    } mode;

    Window& m_window;
    glm::vec2 m_screenSize {};
    glm::vec2 m_onScreenPosition {}; // pixelPerfect, clamped to screen position, mostly set by user

    glm::vec2 m_systemPointerPosition {};
    glm::vec2 m_positionDelta {};
    glm::vec2 m_velocity {};

    glm::dvec2 m_lastPosition {};

    bool m_wrapPosition {true};
    bool m_useSystemPointer {false};
    bool m_doNotRenderGamePointer {false};

    glm::vec2 wrap(glm::vec2 in) const {
        return in;
    }
    // todo: sensitivity, najlepiej stos updatów, żeby dało się przełączać pomiędzy róznymi czułościami
public:
    InputUserPointer(Window& window, glm::vec2 screenSize);

    struct
    {
        glm::vec4 position;
        glm::vec4 normal;
    } world;

    void update(float dt);
    glm::vec2 screenScale() const;

    void hideSystemCursor();
    void showSystemCursor();
    void hidePointer() {
        m_doNotRenderGamePointer = true;
    }
    void showPointer() {
        m_doNotRenderGamePointer = false;
        setCentered();
    }
    void setVisibility(bool isVisible) {
        m_doNotRenderGamePointer = !isVisible;
    }
    bool systemMode() const {
        return m_useSystemPointer;
    }
    bool gameMode() const {
        return !m_useSystemPointer;
    }

    bool visible() const {
        return not m_useSystemPointer and not m_doNotRenderGamePointer;
    }
    const glm::vec2& screenPosition() const {
        if(systemMode())
            return m_systemPointerPosition;
        return m_onScreenPosition;
    }
    const glm::vec2& delta() const {
        return m_positionDelta;
    }

    void setSystemPosition(glm::vec2 position) {
        m_systemPointerPosition = position;
    }

    void setDelta(glm::vec2 p_delta) {
        m_positionDelta = p_delta;
        if(systemMode())
            return;
        m_onScreenPosition += p_delta;
    }

    // to be called each frame to update in-game pointer position
    void setCentered(glm::vec2 position = glm::vec2(0.5f)) {
        m_onScreenPosition = wrap(m_onScreenPosition);
    }
    void setFromWorldPosition(const glm::vec4& worldPosition, const glm::mat4& viewMatrix) {
        m_onScreenPosition = glm::project(worldPosition.xyz(), glm::mat4(1), viewMatrix, glm::vec4(0, 0, m_screenSize));
    }
    void setFromGame(glm::vec2 calculatedPosition) {
        m_onScreenPosition = calculatedPosition;
    }
};