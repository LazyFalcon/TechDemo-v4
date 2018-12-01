#pragma once
#include "IModule.hpp"

class DriveSystem : public IModule
{
public:
    DriveSystem(VehicleEquipment &eq) : IModule(eq, ModuleType::DriveSystem){}

};
