#pragma once

class IModule;
class IMotor;
class PhysicalWorld;
class Tank;
class TrackSim;
class VehicleEquipment;
class Yaml;

class ModuleFactory
{
    VehicleEquipment &m_vehicleEq;
    PhysicalWorld &physics;
    btVector3 startPosition;
    int weaponId {0};

public:
    ModuleFactory(VehicleEquipment &eq, PhysicalWorld &physics, glm::vec4 startPosition);

    std::shared_ptr<IMotor> createDieselMotor(const Yaml &cfg);
    std::shared_ptr<IMotor> createElectricMotor(const Yaml &cfg);
    std::shared_ptr<IModule> createModule(const Yaml &cfg);
    std::shared_ptr<IModule> createBase(const Yaml &cfg);
    std::shared_ptr<IModule> createTurret(const Yaml &cfg);
    std::shared_ptr<IModule> createAddon(const Yaml &cfg);
    std::shared_ptr<IModule> createArmor(const Yaml &cfg);
    std::shared_ptr<IModule> createPowerShield(const Yaml &cfg);
    std::shared_ptr<IModule> createHeadlight(const Yaml &cfg);
    std::shared_ptr<IModule> createLoosePart(const Yaml &cfg);

    btRigidBody *vehicle;
};
