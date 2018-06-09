#include "core.hpp"
#include "ui.hpp"
#include "input.hpp"
#include "input-dispatcher.hpp"
#include "Player.hpp"


Player::Player(InputDispatcher& inputDispatcher) :
    mouseSampler(std::make_unique<GBufferSampler>()),
    crosshairSampler(std::make_unique<GBufferSampler>()),
    m_input(inputDispatcher.createNew("Player"))
    {
        mouseSampler->samplePosition = glm::vec2(0,0);
        crosshairSampler->samplePosition = glm::vec2(0.5);
        crosshairSampler->relativePosition = true;

        initInputContext();
    };
Player::~Player(){
}

void Player::initInputContext(){
    m_input->action("W").name("Forward").hold([this]{
            // controlYValue += 0.1f * (1-controlYValue);
            controlYValue += 0.01;
            controlYValue = glm::clamp(controlYValue, -1.f, 1.f);
        }).off([this]{
            controlYValue = 0;
        } );
    m_input->action("S").name("Backward").hold([this]{
            // controlYValue += 0.1f * (controlYValue-1);
            controlYValue -= 0.01;
            controlYValue = glm::clamp(controlYValue, -1.f, 1.f);
        }).off([this]{
            controlYValue = 0;
        });
    m_input->action("D").name("Turn right").hold([this]{
            controlXValue += 0.01f;
            controlXValue = glm::clamp(controlXValue, 0.f, 1.f);
            // controlXValue = std::max(1.f, 0.01f * (1-controlXValue));
        }).off([this]{
            controlXValue = 0;
        });
    m_input->action("A").name("Turn left").hold([this]{
            controlXValue -= 0.01f;
            controlXValue = glm::clamp(controlXValue, -1.f, 0.f);
            // controlXValue = std::min(-1.f, 0.01f * (controlXValue-1));
        }).off([this]{
            controlXValue = 0;
        });
    // m_input->action("ctrl-x").name("lock guns").on([this]{ m_vehicleEq.lockCannonsInDefaultPosition(); });
    m_input->action("ctrl-c").name("lock guns on point").on([this]{ isLockedOnPoint = !isLockedOnPoint; });
    m_input->action("RMB").name("Fire").hold([this]{ doFire = true; });
    m_input->action("[").on([this]{ nextCamera(); });
    m_input->action("]").on([this]{ prevCamera(); });
    m_input->action("P").name("Lost focus").on([this]{ focusOff(); });

    m_input->activate();
}

void Player::updateGraphic(float dt){
    m_vehicleEq.updateMarkers();
    m_vehicleEq.drawBBOXesOfChildren();
    // updateCameras(dt);
    graphics.toBeRendered();
}
void Player::update(float dt){
    // mouseSampler->samplePosition = KeyState::mousePosition;

    // crosshair = m_vehicleEq.cameras[cameraId]->focusPoint;
    // if(not isLockedOnPoint) targetPointPosition = convert(crosshairSampler->position);

    // // m_vehicleEq.setTargetPoint(targetPointPosition, 0);
    // m_vehicleEq.sko->updateTarget(convert(targetPointPosition, 1));
    // m_vehicleEq.updateModules(dt);
    // m_vehicleEq.driveSystem->update(controlXValue, controlYValue, dt);
    // m_vehicleEq.compound->recalculateLocalAabb();

    // for(auto &it : m_vehicleEq.weapons) it.update(dt);

    // if(doFire) fire();
}

void Player::lockInDefaultPosition(){
    // btVector3 target = m_vehicleEq.rgBody->getCenterOfMassPosition();
    // target += m_vehicleEq.rgBody->getCenterOfMassTransform().getBasis().getColumn(1);
}
void Player::updateCameras(float dt){
    for(auto &camera : m_vehicleEq.cameras)
        camera->update(dt);
}

void Player::focusOn(){
    // m_vehicleEq.cameras[cameraId]->focus();
    // input->activate();
}
void Player::focusOff(){
    // input->deactivate();
}

void Player::fire(){
    // for(auto &it : m_vehicleEq.weapons){
    //     if(not it.fire()) clog("Unable to fire");
    // }
    // doFire = false;
}
