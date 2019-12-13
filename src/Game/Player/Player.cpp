#include "core.hpp"
#include "Player.hpp"
#include "input-dispatcher.hpp"
#include "input-user-pointer.hpp"
#include "input.hpp"
#include "ui.hpp"

Player::Player(InputDispatcher& inputDispatcher, Vehicle& vehicle, InputUserPointer& userPointer)
    : BaseOfGameObject(GameType::Actor),
      m_input(inputDispatcher.createNew("Player")),
      m_vehicle(vehicle),
      m_userPointer(userPointer) {
    initInputContext();
};
Player::~Player() {}

void Player::initInputContext() {
    m_input->action("W").name("Forward").hold([this] { m_vehicle.control.controlOnAxes.y = 1; }).off([this] {
        m_vehicle.control.controlOnAxes.y = 0;
    });
    m_input->action("S").name("Backward").hold([this] { m_vehicle.control.controlOnAxes.y = -1; }).off([this] {
        m_vehicle.control.controlOnAxes.y = 0;
    });
    m_input->action("D").name("Strafe right").hold([this] { m_vehicle.control.controlOnAxes.x = 1; }).off([this] {
        m_vehicle.control.controlOnAxes.x = 0;
    });
    m_input->action("A").name("Strafe left").hold([this] { m_vehicle.control.controlOnAxes.x = -1; }).off([this] {
        m_vehicle.control.controlOnAxes.x = 0;
    });
    m_input->action("space").name("Up").hold([this] { m_vehicle.control.controlOnAxes.z = 1; }).off([this] {
        m_vehicle.control.controlOnAxes.z = 0;
    });
    m_input->action("C").name("Down").hold([this] { m_vehicle.control.controlOnAxes.z = -1; }).off([this] {
        m_vehicle.control.controlOnAxes.z = 0;
    });
    // m_input->action("ctrl-x").name("lock guns").on([this]{ m_vehicle.lockCannonsInDefaultPosition(); });
    m_input->action("ctrl-c").name("lock guns on point").on([this] {
        if(m_lockedOnPosition)
            m_lockedOnPosition.reset();
        else
            m_lockedOnPosition = m_userPointer.world.position;
    });
    m_input->action("LMB").name("Fire").hold([this] { doFire = true; });
    m_input->action("[").on([this] { m_vehicle.cameras.prev(); });
    m_input->action("]").on([this] { m_vehicle.cameras.next(); });
    m_input->activate();
}

void Player::updateGraphic(float dt) {
    m_vehicle.updateMarkers();
    m_vehicle.drawBBOXesOfChildren();
    m_vehicle.graphics.toBeRendered();
}
void Player::update(float dt) {
    if(m_isFocusOnPlayer) {
        targetPointPosition = m_lockedOnPosition ? *m_lockedOnPosition : m_userPointer.world.position;
    }
    if(glm::distance(m_vehicle.getPosition(), targetPointPosition) >= 20.f) {
        m_vehicle.fireControlUnit->updateTarget(targetPointPosition);
        m_vehicle.control.aimingAt = targetPointPosition;
    }
    // m_vehicle.fireControlUnit->updateTarget(glm::vec4(200,200,90,1));

    // for(auto &it : m_vehicle.weapons) it.update(dt);
    m_vehicle.updateModules(dt);
    updateCameras(dt);
    if(doFire)
        fire();
}

void Player::lockInDefaultPosition() {
    // btVector3 target = m_vehicle.rgBody->getCenterOfMassPosition();
    // target += m_vehicle.rgBody->getCenterOfMassTransform().getBasis().getColumn(1);
}
void Player::updateCameras(float dt) {
    // for(auto& camera : m_vehicle.cameras) camera->updatePositionAndRotation(dt);
}

void Player::focusOn() {
    m_isFocusOnPlayer = true;
    m_vehicle.cameras.focus();
    m_input->activate();
}
void Player::focusOff() {
    m_isFocusOnPlayer = false;
    m_input->deactivate();
}

void Player::fire() {
    // for(auto &it : m_vehicle.weapons){
    //     if(not it.fire()) console.clog("Unable to fire");
    // }
    doFire = false;
}
