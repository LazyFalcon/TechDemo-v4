#pragma once
#include "Actor.hpp"
#include "Utils.hpp"
#include "Vehicle.hpp"

class Cannon;
class Imgui;
class Input;
class InputDispatcher;
class InputUserPointer;

class Player : public Actor
{
private:
    std::shared_ptr<Input> m_input;
    Vehicle& m_vehicle;
    InputUserPointer& m_userPointer;

    std::optional<glm::vec4> m_lockedOnPosition;
    glm::vec4 targetPointPosition {0, 1000, 0, 1};

    int cameraId {0};
    glm::vec2 crosshair;

    bool m_isFocusOnPlayer {false};
    bool doFire = false;
    void fire();
    void updateCameras(float dt);

    void setTarget(btVector3 t);
    void lockInDefaultPosition();

    void initInputContext();
    void looseFocus();

public:
    Player(InputDispatcher&, Vehicle&, InputUserPointer&);
    ~Player();
    void update(float dt);
    void updateGraphic(float dt);
    void focusOn();
    void focusOff();
    bool hasFocus() {
        return m_isFocusOnPlayer;
    }
    Vehicle& eq() {
        return m_vehicle;
    }
};
