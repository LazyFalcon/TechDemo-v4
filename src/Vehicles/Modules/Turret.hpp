#pragma once
#include "IModule.hpp"
#include "Joint.hpp"

/*
* moving part of gun, responsible for calculations of rotation angle(later move to SKO)
* rename later.
*/
class GunMovingPart : public IModule
{
public:
    GunMovingPart(VehicleEquipment &eq) : IModule(eq, ModuleType::GunMovingPart){}
    void update(float dt) override;
};

class Gun : public IModule
{
public:
    Gun(VehicleEquipment &eq) : IModule(eq, ModuleType::Gun){}
    void update(float dt) override;
    GunBase& baseRef;
};
