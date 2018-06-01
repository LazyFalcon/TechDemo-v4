#pragma once
#include "Includes.hpp"

class IModule;
class IMotor;
class PhysicsWorld;
class Tank;
class TrackSim;
class Yaml;

class ModuleFactory
{
    VehicleEquipment &eq;
    PhysicsWorld &physics;
    btVector3 startPosition;
    int weaponId {0};

public:
    ModuleFactory(VehicleEquipment &eq, PhysicsWorld &physics, glm::vec4 startPosition);

    shared_ptr<IMotor> createDieselMotor(const Yaml &cfg);
    shared_ptr<IMotor> createElectricMotor(const Yaml &cfg);
    shared_ptr<IModule> createModule(const Yaml &cfg);
    shared_ptr<IModule> createBase(const Yaml &cfg);
    shared_ptr<IModule> createTurret(const Yaml &cfg);
    shared_ptr<IModule> createMantlet(const Yaml &cfg);
    shared_ptr<IModule> createCannon(const Yaml &cfg);
    shared_ptr<IModule> createAddon(const Yaml &cfg);
    shared_ptr<IModule> createArmor(const Yaml &cfg);
    shared_ptr<IModule> createPowerShield(const Yaml &cfg);
    shared_ptr<IModule> createHeadlight(const Yaml &cfg);

    btRigidBody *vehicle;
};
