#include "core.hpp"
#include "Modules.hpp"
#include "Logging.hpp"

void Base::update(float dt){
    btTransform btTr;
    eq.rgBody->getMotionState()->getWorldTransform(btTr);

    auto tr = convert(btTr);
    eq.invTrans = glm::inverse(tr);
    eq.glTrans = tr;
    this->transform(tr);
    clog("Drone position", tr[3]);
}
