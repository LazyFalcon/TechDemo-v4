#pragma once
#include "IModule.hpp"

class Base : public IModule
{
    Base(VehicleEquipment &eq) : IModule(eq, ModuleType::Base){}
    void update(float dt) override;
};
