#include "core.hpp"
#include "Turret.hpp"
#include "Logging.hpp"

void TurretPart::update(float dt){
    clog(__PRETTY_FUNCTION__, joint->getTransform()[3]);
    this->transform(joint->getTransform());
}

void Turret::update(float dt){
    clog(__PRETTY_FUNCTION__, joint->getTransform()[3]);
    this->transform(joint->getTransform());
}
