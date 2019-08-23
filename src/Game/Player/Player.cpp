#include "core.hpp"
#include "ui.hpp"
#include "input.hpp"
#include "input-dispatcher.hpp"
#include "Player.hpp"


Player::Player(InputDispatcher& inputDispatcher, Vehicle& vehicle) :
    m_input(inputDispatcher.createNew("Player")),
    m_vehicle(vehicle),
    mouseSampler(std::make_unique<GBufferSampler>()),
    crosshairSampler(std::make_unique<GBufferSampler>())
    {
        // todo: wywalić to na rzecz wspólnego stanu myszy
        mouseSampler->samplePosition = glm::vec2(0,0);
        crosshairSampler->samplePosition = glm::vec2(0.5f, 0.7f);
        crosshairSampler->relativePosition = true;

        initInputContext();
    };
Player::~Player(){
}

void Player::initInputContext(){
    m_input->action("W").name("Forward").hold([this]{
            m_vehicle.control.controlOnAxes.y = 1;
        }).off([this]{
            m_vehicle.control.controlOnAxes.y = 0;
        });
    m_input->action("S").name("Backward").hold([this]{
            m_vehicle.control.controlOnAxes.y = -1;
        }).off([this]{
            m_vehicle.control.controlOnAxes.y = 0;
        });
    m_input->action("D").name("Strafe right").hold([this]{
            m_vehicle.control.controlOnAxes.x = 1;
        }).off([this]{
            m_vehicle.control.controlOnAxes.x = 0;
        });
    m_input->action("A").name("Strafe left").hold([this]{
            m_vehicle.control.controlOnAxes.x = -1;
        }).off([this]{
            m_vehicle.control.controlOnAxes.x = 0;
        });
    m_input->action("space").name("Up").hold([this]{
            m_vehicle.control.controlOnAxes.z = 1;
        }).off([this]{
            m_vehicle.control.controlOnAxes.z = 0;
        });
    m_input->action("C").name("Down").hold([this]{
            m_vehicle.control.controlOnAxes.z = -1;
        }).off([this]{
            m_vehicle.control.controlOnAxes.z = 0;
        });
    // m_input->action("ctrl-x").name("lock guns").on([this]{ m_vehicle.lockCannonsInDefaultPosition(); });
    m_input->action("ctrl-c").name("lock guns on point").on([this]{ isLockedOnPoint = !isLockedOnPoint; });
    m_input->action("LMB").name("Fire").hold([this]{ doFire = true; });
    m_input->action("[").on([this]{ m_vehicle.cameras.prev(); });
    m_input->action("]").on([this]{ m_vehicle.cameras.next(); });
    m_input->activate();
}

void Player::updateGraphic(float dt){
    m_vehicle.updateMarkers();
    m_vehicle.drawBBOXesOfChildren();
    updateCameras(dt);
    m_vehicle.graphics.toBeRendered();
}
void Player::update(float dt){
    mouseSampler->samplePosition = m_input->getMouseState().pointerScreenPosition;

    // crosshair = m_vehicle.cameras[cameraId]->focusPoint;
    // todo: określić czy celownik mamy na myszy czy niezależnie na środku ekranu,
    // wtedy też bedziemy to pobierać ze stanu myszy
    if(not isLockedOnPoint) targetPointPosition = m_hasFocus ? crosshairSampler->position : mouseSampler->position;

    if(glm::distance(m_vehicle.getPosition(), targetPointPosition) >= 20.f){
        m_vehicle.fireControlUnit->updateTarget(targetPointPosition);
        m_vehicle.control.aimingAt = targetPointPosition;
    }
    // m_vehicle.fireControlUnit->updateTarget(glm::vec4(200,200,90,1));
    m_vehicle.updateModules(dt);
    // m_vehicle.driveSystem->update(controlXValue, controlYValue, dt);
    // m_vehicle.compound->recalculateLocalAabb();

    // for(auto &it : m_vehicle.weapons) it.update(dt);

    // if(doFire) fire();
}

void Player::lockInDefaultPosition(){
    // btVector3 target = m_vehicle.rgBody->getCenterOfMassPosition();
    // target += m_vehicle.rgBody->getCenterOfMassTransform().getBasis().getColumn(1);
}
void Player::updateCameras(float dt){
    for(auto &camera : m_vehicle.cameras)
        camera->update(dt);
}

void Player::focusOn(){
    m_hasFocus = true;
    m_vehicle.cameras.focus();
    m_input->activate();
}
void Player::focusOff(){
    m_hasFocus = false;
    m_input->deactivate();
}

void Player::fire(){
    // for(auto &it : m_vehicle.weapons){
    //     if(not it.fire()) console.clog("Unable to fire");
    // }
    // doFire = false;
}
