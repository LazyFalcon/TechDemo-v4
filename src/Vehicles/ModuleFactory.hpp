#pragma once
#include "ModuleFactory.hpp"
#include "Logging.hpp"
#include "Modules.hpp"
#include "Turret.hpp"
#include "Yaml.hpp"
#include "VehicleEquipment.hpp"

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
    ModuleFactory(VehicleEquipment &eq, PhysicalWorld &physics, glm::vec4 startPosition)
    : m_vehicleEq(eq), physics(physics), startPosition(convert(startPosition)) {}

    btRigidBody *vehicle;

    std::shared_ptr<IMotor> createDieselMotor(const Yaml &cfg){
        return nullptr;
    }
    std::shared_ptr<IMotor> createElectricMotor(const Yaml &cfg){
        return nullptr;
    }

    std::shared_ptr<IModule> createModule(const Yaml &cfg){
        std::string className = cfg["Class"].string();

        if(className == "Hull"){
            return createBase(cfg);
        }
        else if(className == "TurretBase"){
            return createTurret(cfg);
        }
        else if(className == "TurretPart"){
            return std::make_shared<TurretPart>(m_vehicleEq);
        }
        else if(className == "Gun"){
            return createAddon(cfg);
        }
        else if(className == "Addon"){
            return createAddon(cfg);
        }
        else if(className == "LoosePart"){
            return createLoosePart(cfg);
        }
        else if(className == "Armor"){
            return createArmor(cfg);
        }

        error("Module", cfg["Name"].string(), "has undefined type:", className);
        return nullptr;
    }
    std::shared_ptr<IModule> createBase(const Yaml &cfg){
        auto ptr = std::make_shared<Base>(m_vehicleEq);

        return ptr;
    }
    std::shared_ptr<IModule> createTurret(const Yaml &cfg){
        auto ptr = std::make_shared<Turret>(m_vehicleEq);

        return ptr;
    }
    std::shared_ptr<IModule> createAddon(const Yaml &cfg){
        auto ptr = std::make_shared<Addon>(m_vehicleEq);

        return ptr;
    }
    std::shared_ptr<IModule> createArmor(const Yaml &cfg){
        auto ptr = std::make_shared<Armor>(m_vehicleEq);

        return ptr;
    }
    std::shared_ptr<IModule> createPowerShield(const Yaml &cfg){
        auto ptr = std::make_shared<Armor>(m_vehicleEq);

        return ptr;
    }
    std::shared_ptr<IModule> createHeadlight(const Yaml &cfg){
        auto ptr = std::make_shared<Headlight>(m_vehicleEq);

        return ptr;
    }
    std::shared_ptr<IModule> createLoosePart(const Yaml &cfg){
        auto ptr = std::make_shared<LoosePart>(m_vehicleEq);

        return ptr;
    }
};
