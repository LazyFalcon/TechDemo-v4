#include "core.hpp"
#include "Modules.hpp"
#include "Logger.hpp"

void Hull::update(float dt){
    btTransform btTr;
    vehicle.rgBody->getMotionState()->getWorldTransform(btTr);
    // btTransform btTr = vehicle.rgBody->getCenterOfMassTransform();

    auto tr = convert(btTr);
    vehicle.invTrans = glm::inverse(tr);
    vehicle.glTrans = tr;
    vehicle.btTrans = btTr;
    this->transform(tr);
    console.flog(tr[3]);
    console.clog(tr[3]);
}
