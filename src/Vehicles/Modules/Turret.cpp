#include "Turret.hpp"

void TurretPart::update(float dt){
    this->transform(joint.loc());
}

void Turret::update(float dt){
    this->transform(joint.loc());
}
