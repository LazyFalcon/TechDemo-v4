#pragma once
#include "Actor.hpp"

class Actor;
class ICameras;
class ICannon;
class VehicleEquipment;
class WeaponHolder;

class Actor
{
public:
    virtual ~Actor() = default;
    virtual void update(float dt) = 0;
    virtual void updateGraphic(float dt) = 0;
    virtual void focusOn() = 0;
    virtual void focusOff() = 0;
    virtual VehicleEquipment& eq() = 0;
};
