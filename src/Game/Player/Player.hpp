#pragma once
#include "Actor.hpp"
#include "CameraController.hpp"
#include "GBufferSampler.hpp"
#include "Utils.hpp"
#include "VehicleEquipment.hpp"

class Cannon;
class Tank;
class Imgui;
class Input;
class InputDispatcher;

class Player : public Actor
{
private:
    std::shared_ptr<Input> m_input;
    VehicleEquipment m_vehicle;
    std::unique_ptr<GBufferSampler> mouseSampler;
    std::unique_ptr<GBufferSampler> crosshairSampler;

    int cameraId {0};
    glm::vec2 crosshair;

    btVector3 targetPointPosition {0,1000,0};
    bool isLockedOnPoint { false };

    float controlXValue {0};
    float controlYValue {0};


    // bool processHit(projectiles::Projectile &p);
    bool doFire = false;
    void fire();
    void nextCamera(){
        cyclicDecr(cameraId, m_vehicle.cameras.size());
    }
    void prevCamera(){
        cyclicIncr(cameraId, m_vehicle.cameras.size());
    }
    Camera& getCamera(){
        return *(m_vehicle.cameras[cameraId]);
    }
    CameraController& getCameraController(){
        return *(m_vehicle.cameras[cameraId]);
    }
    void updateCameras(float dt);

    void setTarget(btVector3 t);
    void lockInDefaultPosition();

    void initInputContext();
    void looseFocus();
public:
    Player(InputDispatcher&, VehicleEquipment&);
    ~Player();
    void update(float dt);
    void updateGraphic(float dt);
    void focusOn();
    void focusOff();
    VehicleEquipment& eq(){
        return m_vehicle;
    }


};
