#include "core.hpp"
#include "Modules.hpp"
#include "Logger.hpp"

void Base::update(float dt){
    btTransform btTr;
    eq.rgBody->getMotionState()->getWorldTransform(btTr);

    auto tr = convert(btTr);
    eq.invTrans = glm::inverse(tr);
    eq.glTrans = tr;
    this->transform(tr);
    console.clog(__PRETTY_FUNCTION__, tr[3]);
}
