#include "core.hpp"
#include "DummyDriveSystem.hpp"
#include "Logging.hpp"

void DummyDriveSystem::update(float dt){
    m_position += m_moveDirection;
}

void DummyDriveSystem::updateInsidePhysicsStep(float dt){
    auto currentForce = eq.rgBody->getTotalForce();
    auto currentTorque = eq.rgBody->getTotalTorque();

    clog("current force:", currentForce.length(), "torque:", currentTorque.length());

    btTransform tr;
    eq.rgBody->getMotionState()->getWorldTransform(tr);
    auto positionError = tr.getOrigin() - m_position;
    auto rotationError = tr.getBasis()[m_leadingAxis].cross(m_lookDirection) + tr.getBasis()[2].cross(btVector3(0,0,1));

    eq.rgBody->applyCentralForce(positionError);
    eq.rgBody->applyTorque(rotationError * 0.3f);
}
