#pragma once
#include "IModule.hpp"
#include "Joint.hpp"

class Base : public IModule
{
public:
    Base(VehicleEquipment &eq) : IModule(eq, ModuleType::Base){}
    void update(float dt) override;
};

class Addon : public IModule
{
public:
    Addon(VehicleEquipment &eq) : IModule(eq, ModuleType::Addon){}
    void update(float dt) override {
        this->transform(joint->getTransform());
    }
};

class LoosePart : public IModule
{
public:
    LoosePart(VehicleEquipment &eq) : IModule(eq, ModuleType::LoosePart){}
    void update(float dt) override {
        this->transform(joint->getTransform());
    }
};

class Armor : public IModule
{
public:
    Armor(VehicleEquipment &eq) : IModule(eq, ModuleType::Armor){}
    void update(float dt) override {}
};

class PowerShield : public IModule
{
public:
    PowerShield(VehicleEquipment &eq) : IModule(eq, ModuleType::Armor){}
    void update(float dt) override {}

    float max;
    float recheargeDelay;
    float recheargeRate;
    float currentValue;
};

class Headlight : public IModule
{
public:
    Headlight(VehicleEquipment &eq) : IModule(eq, ModuleType::Armor){}
    void update(float dt) override {}
};

class Actor;
class Radar : public IModule
{
public:
    Radar(VehicleEquipment &eq) : IModule(eq, ModuleType::Radar){}
    void update(float dt) override {}
    void update(std::vector<std::shared_ptr<Actor>>& actors);

    std::vector<Actor*> nbEnemies;
    std::vector<Actor*> nbAllies;
};
