#pragma once
#include "ModuleFactory.hpp"
#include "Logger.hpp"
#include "Modules.hpp"
#include "WeaponModules.hpp"
#include "Yaml.hpp"
#include "Vehicle.hpp"

class PhysicalWorld;

#define moduleCreateMacro(Type) m_moduleMap[#Type] = [this](const Yaml& cfg, IModule* parent){ return std::make_shared<Type>(#Type, m_vehicleEq, parent); };

class ModuleFactory
{
private:
    Vehicle &m_vehicleEq;
    PhysicalWorld &physics;
    btVector3 startPosition;
    int weaponId {0};
    std::map<std::string, std::function<std::shared_ptr<IModule>(const Yaml&, IModule* parent)>> m_moduleMap;

public:
    btRigidBody *vehicle;

    ModuleFactory(Vehicle &eq, PhysicalWorld &physics, glm::vec4 startPosition)
    : m_vehicleEq(eq), physics(physics), startPosition(convert(startPosition)) {
        moduleCreateMacro(Hull)
        moduleCreateMacro(Turret)
        moduleCreateMacro(GunServo)
        moduleCreateMacro(Gun)
        moduleCreateMacro(Addon)
        moduleCreateMacro(LoosePart)
        moduleCreateMacro(Armor)
    }

    std::shared_ptr<IModule> createModule(const Yaml& cfg, IModule* parent){
        auto type = cfg["Class"].string();
        if(m_moduleMap.count(type) == 0){
            console.error("No module of type", type);
            return {};
        }
        auto out = m_moduleMap[type](cfg, parent);
        console.log("Module of type", type, "named", out->name);
        return out;
    }
};

#undef moduleCreateMacro
