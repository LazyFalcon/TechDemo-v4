#include "core.hpp"
#include "Turret.hpp"
#include "Logger.hpp"

void TurretPart::update(float dt){
    console.clog(__PRETTY_FUNCTION__, joint->getTransform()[3]);
    this->transform(joint->getTransform());
}

void Turret::update(float dt){
    console.clog(__PRETTY_FUNCTION__, joint->getTransform()[3]);
    this->transform(joint->getTransform());
}
