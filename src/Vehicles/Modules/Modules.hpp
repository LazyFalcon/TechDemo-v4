#pragma once
#include "IModule.hpp"

class Hull : public IModule
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
        // this->transform(joint->getTransform());
    }
};

class LoosePart : public IModule
{
public:
    using IModule::IModule;
    void update(float dt) override {
        // this->transform(joint->getTransform());
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
