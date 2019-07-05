#pragma once
#include "ModuleFactory.hpp"
#include "Logger.hpp"
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
private:
    VehicleEquipment &m_vehicleEq;
    PhysicalWorld &physics;
    btVector3 startPosition;
    int weaponId {0};
    std::map<std::string, std::function<std::shared_ptr<IModule>(const Yaml&)>> m_moduleMap;


public:
    btRigidBody *vehicle;

    ModuleFactory(VehicleEquipment &eq, PhysicalWorld &physics, glm::vec4 startPosition)
    : m_vehicleEq(eq), physics(physics), startPosition(convert(startPosition)) {
        m_moduleMap["Hull"] = [this](const Yaml& cfg){ return std::make_shared<Base>(m_vehicleEq); };
        m_moduleMap["TurretBase"] = [this](const Yaml& cfg){ return std::make_shared<Turret>(m_vehicleEq); };
        m_moduleMap["TurretPart"] = [this](const Yaml& cfg){ return std::make_shared<TurretPart>(m_vehicleEq); };
        m_moduleMap["Gun"] = [this](const Yaml& cfg){ return std::make_shared<Gun>(m_vehicleEq); };
        m_moduleMap["Addon"] = [this](const Yaml& cfg){ return std::make_shared<Addon>(m_vehicleEq); };
        m_moduleMap["LoosePart"] = [this](const Yaml& cfg){ return std::make_shared<Addon>(m_vehicleEq); };
        m_moduleMap["Armor"] = [this](const Yaml& cfg){ return std::make_shared<Armor>(m_vehicleEq); };
    }

    std::shared_ptr<IModule> createModule(const Yaml& cfg){
        auto type = cfg["Class"].string();
        if(m_moduleMap.count(type) == 0){
            console.error("No module of type", type);
            return {};
        }

        return m_moduleMap[type](cfg);
    }


};
