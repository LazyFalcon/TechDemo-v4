#pragma once
#include "Actor.hpp"
#include "GBufferSampler.hpp"
#include "Utils.hpp"
#include "Vehicle.hpp"

class Cannon;
class Imgui;
class Input;
class InputDispatcher;

class Player : public Actor
{
private:
    std::shared_ptr<Input> m_input;
    Vehicle& m_vehicle;
    std::unique_ptr<GBufferSampler> mouseSampler;
    std::unique_ptr<GBufferSampler> crosshairSampler;

    int cameraId {0};
    glm::vec2 crosshair;

    glm::vec4 targetPointPosition {0, 1000, 0, 1};
    bool isLockedOnPoint { false };

    float controlXValue {0};
    float controlYValue {0};
    bool m_hasFocus {false};
    // bool processHit(projectiles::Projectile &p);
    bool doFire = false;
    void fire();
    void updateCameras(float dt);

    void setTarget(btVector3 t);
    void lockInDefaultPosition();

    void initInputContext();
    void looseFocus();
public:
    Player(InputDispatcher&, Vehicle&);
    ~Player();
    void update(float dt);
    void updateGraphic(float dt);
    void focusOn();
    void focusOff();
    bool hasFocus(){
        return m_hasFocus;
    }
    Vehicle& eq(){
        return m_vehicle;
    }


};
