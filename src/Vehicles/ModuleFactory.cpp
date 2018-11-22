#include "core.hpp"
#include "ModuleFactory.hpp"
#include "Logging.hpp"
#include "Modules.hpp"
#include "Turret.hpp"
#include "Yaml.hpp"
#include "VehicleEquipment.hpp"

ModuleFactory::ModuleFactory(VehicleEquipment &eq, PhysicalWorld &physics, glm::vec4 startPosition)
: m_vehicleEq(eq), physics(physics), startPosition(convert(startPosition)) {}

std::shared_ptr<IMotor> ModuleFactory::createDieselMotor(const Yaml &cfg){
    return nullptr;
}
std::shared_ptr<IMotor> ModuleFactory::createElectricMotor(const Yaml &cfg){
    return nullptr;
}

std::shared_ptr<IModule> ModuleFactory::createModule(const Yaml &cfg){
    std::string className = cfg["ModuleType"].string();

    if(className == "Base"){
        return createBase(cfg);
    }
    else if(className == "Turret"){
        return createTurret(cfg);
    }
    else if(className == "Turret-Part"){
        return std::make_shared<TurretPart>(m_vehicleEq);
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
std::shared_ptr<IModule> ModuleFactory::createBase(const Yaml &cfg){
    auto ptr = std::make_shared<Base>(m_vehicleEq);

    return ptr;
}
std::shared_ptr<IModule> ModuleFactory::createTurret(const Yaml &cfg){
    auto ptr = std::make_shared<Turret>(m_vehicleEq);

    return ptr;
}
std::shared_ptr<IModule> ModuleFactory::createAddon(const Yaml &cfg){
    auto ptr = std::make_shared<Addon>(m_vehicleEq);

    return ptr;
}
std::shared_ptr<IModule> ModuleFactory::createArmor(const Yaml &cfg){
    auto ptr = std::make_shared<Armor>(m_vehicleEq);

    return ptr;
}
std::shared_ptr<IModule> ModuleFactory::createPowerShield(const Yaml &cfg){
    auto ptr = std::make_shared<Armor>(m_vehicleEq);

    return ptr;
}
std::shared_ptr<IModule> ModuleFactory::createHeadlight(const Yaml &cfg){
    auto ptr = std::make_shared<Headlight>(m_vehicleEq);

    return ptr;
}
std::shared_ptr<IModule> ModuleFactory::createLoosePart(const Yaml &cfg){
    auto ptr = std::make_shared<LoosePart>(m_vehicleEq);

    return ptr;
}
