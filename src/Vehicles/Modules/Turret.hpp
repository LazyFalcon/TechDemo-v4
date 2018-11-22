#pragma once
#include "IModule.hpp"
#include "Joint.hpp"

/*
* dummy module responsible mainly for handling default module behaviors
*/
class TurretPart : public IModule
{

public:
    TurretPart(VehicleEquipment &eq) : IModule(eq, ModuleType::Part){}
    void update(float dt) override;
};

class Weapon;

/*
* class that handles turrets: kinematic module with many guns, handles whole logic internally
* for now max two subturrets and one aimpoint, for more complicated setups go fuck yourself
* make two turrets connected via platform, each turret reports to panel minimal and maximal platform movement that will allow to aim properly
*/
class Turret : public IModule
{
private:
    std::vector<std::shared_ptr<Weapon>> m_installedWeapons;
public:
    Turret(VehicleEquipment &eq) : IModule(eq, ModuleType::Turret){}
    void update(float dt) override;

};

/*
* turret implementation with two attached heads, little different way of calculating IK:
* base of turret always rotate in direction of target, I assumed that heads are somehow symmetrical
*/
class MultiHeadTurret : public IModule
{
private:
public:
    MultiHeadTurret(VehicleEquipment &eq) : IModule(eq, ModuleType::Turret){}
    void update(float dt) override {}

};
