#pragma once
#include "CameraController.hpp"
#include "GBufferSampler.hpp"
#include "common.hpp"
#include "Utils.hpp"

class Cannon;
class Tank;
class Imgui;
class Input;
class InputDispatcher;

class Player
{
private:
    std::unique_ptr<GBufferSampler> mouseSampler;
    std::unique_ptr<GBufferSampler> crosshairSampler;

    int cameraId {0};
    glm::vec2 crosshair;

    btVector3 targetPointPosition {0,1000,0};
    bool isLockedOnPoint { false };

    float controlXValue {0};
    float controlYValue {0};

    std::shared_ptr<Input> m_input;

    // bool processHit(projectiles::Projectile &p);
    bool doFire = false;
    void fire();
    // CameraController& addCamera(IModule *module, const std::string &mode);
    // void nextCamera(){
    //     // cyclicDecr(cameraId, vehicle.cameras.size());
    // }
    // void prevCamera(){
    //     // cyclicIncr(cameraId, vehicle.cameras.size());
    // }
    // Camera& getCamera(){
    //     // return vehicle.cameras[cameraId]->camera;
    // }
    // CameraController& getCameraController(){
    //     // return *vehicle.cameras[cameraId];
    // }
    void updateCameras(float dt);

    void setTarget(btVector3 t);
    void lockInDefaultPosition();

    void initInputContext();
    void looseFocus();
public:
    Player(InputDispatcher&);
    ~Player();
    void update(float dt);
    void updateGraphic(float dt);
    void focusOn();
    void focusOff();
};
