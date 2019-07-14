#include "core.hpp"
#include "Modules.hpp"
#include "Logger.hpp"

void Base::update(float dt){
    btTransform btTr;
    vehicle.rgBody->getMotionState()->getWorldTransform(btTr);

    auto tr = convert(btTr);
    vehicle.invTrans = glm::inverse(tr);
    vehicle.glTrans = tr;
    this->transform(tr);
    console.clog(__PRETTY_FUNCTION__, tr[3]);
}

void GunMovingPart::update(float dt){
    console.clog(__PRETTY_FUNCTION__, joint->getTransform()[3]);

    // auto target = vehicle.vehicleControlUnit->getTarget();

    // joint->IK_targetAtPoint(getTransform(), target);

    this->transform(joint->getTransform());
}

void Gun::update(float dt){
    console.clog(__PRETTY_FUNCTION__, joint->getTransform()[3]);
    this->transform(joint->getTransform());
}
