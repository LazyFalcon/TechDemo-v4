#pragma once
#include "Logger.hpp"
#include "ModuleFactory.hpp"
#include "Modules.hpp"
#include "Vehicle.hpp"
#include "WeaponModules.hpp"
#include "Yaml.hpp"

class PhysicalWorld;

#define moduleCreateMacro(Type)                                     \
    m_moduleMap[#Type] = [this](const Yaml& cfg, IModule* parent) { \
        return std::make_shared<Type>(#Type, m_vehicleEq, parent);  \
    };

class ModuleFactory
{
private:
    Vehicle& m_vehicleEq;
    PhysicalWorld& physics;
    btVector3 startPosition;
    int weaponId {0};
    std::map<std::string, std::function<std::shared_ptr<IModule>(const Yaml&, IModule* parent)>> m_moduleMap;

public:
    btRigidBody* vehicle;

    ModuleFactory(Vehicle& eq, PhysicalWorld& physics, glm::vec4 startPosition)
        : m_vehicleEq(eq), physics(physics), startPosition(convert(startPosition)) {
        moduleCreateMacro(Hull) m_moduleMap["Turret"] = [this](const Yaml& params, IModule* parent) {
            return std::make_shared<Turret>("Turret", m_vehicleEq, parent, params);
        };
        m_moduleMap["GunServo"] = [this](const Yaml& params, IModule* parent) {
            return std::make_shared<GunServo>("GunServo", m_vehicleEq, parent, params);
        };
        moduleCreateMacro(Gun) moduleCreateMacro(Addon) moduleCreateMacro(LoosePart) moduleCreateMacro(Armor)
    }

    std::shared_ptr<IModule> createModule(const Yaml& cfg, IModule* parent) {
        auto type = cfg["Class"].string();
        if(m_moduleMap.count(type) == 0) {
            console.error("No module of type", type);
            return {};
        }
        auto out = m_moduleMap[type](cfg, parent);
        console.log("Module of type", type, "named", out->name);
        return out;
    }
};

#undef moduleCreateMacro
