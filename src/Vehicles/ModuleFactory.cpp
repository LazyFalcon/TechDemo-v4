#include <string>
#include "ModuleFactory.hpp"
#include "Logging.hpp"
#include "TankDriveSystem.hpp"
#include "Modules.hpp"
#include "PhysicsWorld.hpp"
#include "Weapon.hpp"


ModuleFactory::ModuleFactory(VehicleEquipment &eq, PhysicsWorld &physics, glm::vec4 startPosition)
: eq(eq), physics(physics), startPosition(convert(startPosition)) {}

shared_ptr<IMotor> ModuleFactory::createDieselMotor(const Yaml &cfg){
    return nullptr;
}
shared_ptr<IMotor> ModuleFactory::createElectricMotor(const Yaml &cfg){
    return nullptr;
}

shared_ptr<IModule> ModuleFactory::createModule(const Yaml &cfg){
    std::string className = cfg["Class"].string();

    if(className == "Base"){
        return createBase(cfg);
    }
    else if(className == "Turret"){
        return createTurret(cfg);
    }
    else if(className == "Mantlet"){
        return createMantlet(cfg);
    }
    else if(className == "Gun"){
        return createCannon(cfg);
    }
    else if(className == "Addon"){
        return createAddon(cfg);
    }
    else if(className == "Armor"){
        return createArmor(cfg);
    }

    error("Module", cfg["Name"].string(), "has undefined class:", className);
    return nullptr;
}
shared_ptr<IModule> ModuleFactory::createBase(const Yaml &cfg){
    auto ptr = make_shared<Base>(eq);

    return ptr;
}
shared_ptr<IModule> ModuleFactory::createTurret(const Yaml &cfg){
    auto ptr = make_shared<Turret>(eq);

    return ptr;
}
shared_ptr<IModule> ModuleFactory::createMantlet(const Yaml &cfg){
    auto ptr = make_shared<Mantlet>(eq);

    return ptr;
}
shared_ptr<IModule> ModuleFactory::createCannon(const Yaml &cfg){
    auto ptr = make_shared<Cannon>(eq);
    return ptr;
}
shared_ptr<IModule> ModuleFactory::createAddon(const Yaml &cfg){
    auto ptr = make_shared<Addon>(eq);

    return ptr;
}
shared_ptr<IModule> ModuleFactory::createArmor(const Yaml &cfg){
    auto ptr = make_shared<Armor>(eq);

    return ptr;
}
shared_ptr<IModule> ModuleFactory::createPowerShield(const Yaml &cfg){
    auto ptr = make_shared<Armor>(eq);

    return ptr;
}
shared_ptr<IModule> ModuleFactory::createHeadlight(const Yaml &cfg){
    auto ptr = make_shared<Headlight>(eq);

    return ptr;
}
