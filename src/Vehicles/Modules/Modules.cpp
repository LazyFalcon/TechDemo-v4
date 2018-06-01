#include "Modules.hpp"

void Base::update(float dt){
    auto tr = convert(eq.rgBody->getWorldTransform());
    eq.invTrans = glm::inverse(tr);
    eq.glTrans = tr;
    this->transform(tr);
}
