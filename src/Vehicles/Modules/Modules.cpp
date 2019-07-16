#include "core.hpp"
#include "Modules.hpp"
#include "Logger.hpp"

void Hull::update(float dt){
    btTransform btTr;
    vehicle.rgBody->getMotionState()->getWorldTransform(btTr);

    auto tr = convert(btTr);
    vehicle.invTrans = glm::inverse(tr);
    vehicle.glTrans = tr;
    this->transform(tr);
    console.clog(__PRETTY_FUNCTION__, tr[3]);
}
