#pragma once
#include "IModule.hpp"

class Turret : public IModule
{
public:
    Turret(const std::string& name, Vehicle &vehicle) : IModule(name, vehicle){}
    void update(float dt) override {
    console.clog(__PRETTY_FUNCTION__, joint->getTransform()[3]);

        // auto target = vehicle.vehicleControlUnit->getTarget();

        // joint->IK_targetAtPoint(getTransform(), target);

        this->transform(joint->getTransform());
    }
};

class GunServo : public IModule
{
public:
    GunServo(const std::string& name, Vehicle &vehicle) : IModule(name, vehicle){}
    void update(float dt) override {
    console.clog(__PRETTY_FUNCTION__, joint->getTransform()[3]);

        // auto target = vehicle.vehicleControlUnit->getTarget();

        // joint->IK_targetAtPoint(getTransform(), target);

        this->transform(joint->getTransform());
    }
};

class Gun : public IModule
{
public:
    Gun(const std::string& name, Vehicle &vehicle) : IModule(name, vehicle){}
    void update(float dt) override {
        console.clog(__PRETTY_FUNCTION__, joint->getTransform()[3]);
        this->transform(joint->getTransform());
    }
    std::string resource;
};
