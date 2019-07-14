#pragma once
#include "IModule.hpp"

class Base : public IModule
{
public:
    using IModule::IModule;
    void update(float dt) override;
};

class Addon : public IModule
{
public:
    using IModule::IModule;
    void update(float dt) override {
        this->transform(joint->getTransform());
    }
};

class LoosePart : public IModule
{
public:
    using IModule::IModule;
    void update(float dt) override {
        this->transform(joint->getTransform());
    }
};

class Armor : public IModule
{
public:
    using IModule::IModule;
    void update(float dt) override {}
};

class PowerShield : public IModule
{
public:
    using IModule::IModule;
    void update(float dt) override {}

    float max;
    float recheargeDelay;
    float recheargeRate;
    float currentValue;
};

class Headlight : public IModule
{
public:
    using IModule::IModule;
    void update(float dt) override {}
};

class Actor;
class Radar : public IModule
{
public:
    using IModule::IModule;
    void update(float dt) override {}
    void update(std::vector<std::shared_ptr<Actor>>& actors);

    std::vector<Actor*> nbEnemies;
    std::vector<Actor*> nbAllies;
};

/*
* moving part of gun, responsible for calculations of rotation angle(later move to SKO)
* rename later.
*/
class GunMovingPart : public IModule
{
public:
    using IModule::IModule;
    void update(float dt) override;
};

class Gun : public IModule
{
public:
    using IModule::IModule;
    void update(float dt) override;
    std::string resource;
};
